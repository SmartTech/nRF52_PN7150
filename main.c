#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_gpiote.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nfc_demo.h"

#define TWI_ADDRESSES      127


static const nrf_drv_twi_t m_twi     = NRF_DRV_TWI_INSTANCE(0);
nrf_drv_timer_t            m_timer0  = NRF_DRV_TIMER_INSTANCE(0);


void twi_init (void)
{
    ret_code_t err_code;

    const nrf_drv_twi_config_t twi_config = {
       .scl                = ARDUINO_SCL_PIN,
       .sda                = ARDUINO_SDA_PIN,
       .frequency          = NRF_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}


void nfc_interrupt(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nfc_rx_ready();
}

void timer_nfc_interrupt(nrf_timer_event_t event_type, void * p_context)
{
    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0: nfc_timer_tick(); break;
        default: break;
    }
}

void nfc_gpio_init()
{
    nrf_drv_gpiote_init();
    nrf_gpio_cfg_output(NFC_RST_PIN);
    // 
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    nrf_drv_timer_init(&m_timer0, &timer_cfg, timer_nfc_interrupt);
    uint8_t _ticks = nrf_drv_timer_ms_to_ticks(&m_timer0, 1);
    nrf_drv_timer_extended_compare(&m_timer0, NRF_TIMER_CC_CHANNEL0, _ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);
    nrf_drv_timer_enable(&m_timer0);
    //
    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    nrf_drv_gpiote_in_init(NFC_INT_PIN, &in_config, nfc_interrupt);
    nrf_drv_gpiote_in_event_enable(NFC_INT_PIN, true);
}

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    //ret_code_t err_code;
    nfc_interface_t r_interface;

    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    
    NRF_LOG_INFO("PN7150 demo");
    NRF_LOG_FLUSH();
    
    NRF_LOG_INFO("Init TWI");
    NRF_LOG_FLUSH();
    twi_init();
    
    NRF_LOG_INFO("NFC DEMO INIT...");
    NRF_LOG_FLUSH();
    nfc_gpio_init();
    if(nfc_demo_init() == -1) {
      NRF_LOG_INFO("failed! Halt MCU!");
      NRF_LOG_FLUSH();
      while(1);
    }
    NRF_LOG_INFO("OK!");
    NRF_LOG_FLUSH();
    
    while (true)
    {
        NRF_LOG_INFO("Discovering device...");
        NRF_LOG_FLUSH();
        nfc_wait_for_discovery_notification(&r_interface);
        //...
        NRF_LOG_INFO("Device Discovered!");
        NRF_LOG_INFO("Process radio...");
        NRF_LOG_FLUSH();
        process_radio(&r_interface);
    }
}

/** @} */
