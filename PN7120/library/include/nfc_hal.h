#ifndef NFC_HAL_H_
#define NFC_HAL_H_
/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define TIMEOUT_INFINITE    0
#define TIMEOUT_100MS       100
#define TIMEOUT_1S          1000
#define TIMEOUT_2S          2000


#define NFC_LOG_INFO(...)                   //NRF_LOG_INFO(__VA_ARGS__)
#define NFC_LOG_FLUSH()                     //NRF_LOG_FLUSH()
#define NFC_LOG_HEXDUMP_INFO(p_data, len)   //NRF_LOG_HEXDUMP_INFO(p_data, len)

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void nfc_hal_init( uint8_t address_id );

void nfc_hal_delay( uint16_t ms );

void nfc_hal_reset( void );

int nfc_hal_write( uint8_t *data_out, uint16_t count );

int nfc_hal_read( uint8_t *data_in, uint16_t *nbytes_read, uint16_t count );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CLICKNAME_HAL_H_ */

/*** End of File **************************************************************/
