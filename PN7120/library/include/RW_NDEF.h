#ifndef RW_NDEF_H_
#define RW_NDEF_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>

/******************************************************************************
* Preprocessor Constants
*******************************************************************************/
#define RW_NDEF_TYPE_T1T    0x1
#define RW_NDEF_TYPE_T2T    0x2
#define RW_NDEF_TYPE_T3T    0x3
#define RW_NDEF_TYPE_T4T    0x4

/******************************************************************************
* Configuration Constants
*******************************************************************************/
#define RW_MAX_NDEF_FILE_SIZE 240


/******************************************************************************
* Typedefs
*******************************************************************************/
typedef void rw_ndef_callback_t( uint8_t *, uint16_t );

/******************************************************************************
* Variables
*******************************************************************************/
extern uint8_t ndef_buffer[RW_MAX_NDEF_FILE_SIZE];
extern rw_ndef_callback_t *p_rw_ndef_pull_cb;

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief rw_ndef_register_pull_callback
 * @param pCb
 */
void rw_ndef_register_pull_callback( void *p_cb );

/**
 * @brief rw_ndef_reset
 * @param type
 */
void rw_ndef_reset( uint8_t type );

/**
 * @brief rw_ndef_read_next
 * @param p_cmd
 * @param cmd_size
 * @param rsp
 * @param p_rsp_size
 */
void rw_ndef_read_next( uint8_t *p_cmd, uint16_t cmd_size,
                        uint8_t *rsp, uint16_t *p_rsp_size );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /*File_H_*/
