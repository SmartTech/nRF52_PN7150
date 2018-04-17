/******************************************************************************
* Includes
*******************************************************************************/
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "nfc_hal.h"

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_twi.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/

#define I2C_READ                    1
#define I2C_WRITE                   0

static uint8_t _i2c_hw_address;

static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(0);

/******************************************************************************
* Function Definitions
*******************************************************************************/

void nfc_hal_init( uint8_t address_id )
{
    _i2c_hw_address = address_id;
    nfc_hal_reset();
}

void nfc_hal_reset()
{
    NFC_LOG_INFO("NFC reset");
    NFC_LOG_FLUSH();
    nrf_gpio_pin_set(NFC_RST_PIN);
    nrf_delay_ms(10);
    nrf_gpio_pin_clear(NFC_RST_PIN);
    nrf_delay_ms(10);
    nrf_gpio_pin_set(NFC_RST_PIN);
    nrf_delay_ms(10);
}

void nfc_hal_delay( uint16_t ms )
{
   nrf_delay_ms(ms);
}

int nfc_hal_write( uint8_t *data_out, uint16_t count )
{
    ret_code_t err_code;
    uint8_t *buffer = data_out;

    if( buffer == NULL || count < 2 ) return -1;
    
    NFC_LOG_INFO("I2C_write:");
    NFC_LOG_HEXDUMP_INFO(data_out, count);
    NFC_LOG_FLUSH();
    err_code = nrf_drv_twi_tx(&m_twi, _i2c_hw_address, data_out, count, false);
    nfc_hal_delay(10);
    if(err_code==NRF_SUCCESS) {
      NFC_LOG_INFO("I2C_write OK!");
      NFC_LOG_FLUSH();
    }
    else {
      NFC_LOG_INFO("I2C_write ERROR");
      NFC_LOG_FLUSH();
      return 1;
    }
    return 0;
}

int nfc_hal_read( uint8_t *data_in, uint16_t *nbytes_read, uint16_t count )
{
    ret_code_t err_code;
    uint8_t *buffer = data_in;
    
    err_code = nrf_drv_twi_rx(&m_twi, _i2c_hw_address, buffer, 3);
    
    if(err_code==NRF_SUCCESS) {
      NFC_LOG_INFO("I2C_read OK!");
      NFC_LOG_HEXDUMP_INFO(buffer, 3);
    }
    else {
      NFC_LOG_INFO("I2C_read ERROR");
      NFC_LOG_FLUSH();
      return 1;
    }
    
    if( *( buffer + 2 ) != 0 ) {
        *nbytes_read = *( buffer + 2 ) + 3;
        err_code = nrf_drv_twi_rx(&m_twi, _i2c_hw_address, buffer + 3, *( buffer + 2 ));
        if(err_code==NRF_SUCCESS) {
          NFC_LOG_HEXDUMP_INFO(buffer + 3, *( buffer + 2 ));
        }
        else {
          NFC_LOG_INFO("I2C_read ERROR");
          NFC_LOG_FLUSH();
          return 1;
        }
    }
    else {
        *nbytes_read = 3;
    }
    NFC_LOG_FLUSH();
    
    return 0;
}

/*************** END OF FUNCTIONS *********************************************/