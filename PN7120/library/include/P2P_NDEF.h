#ifndef P2P_NDEF_H_
#define P2P_NDEF_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
* Configuration Constants
*******************************************************************************/
#define P2P_NDEF_MAX_NDEF_RECORD_SIZE 240


/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void p2p_ndef_reset( void );
void p2p_ndef_next( uint8_t *p_cmd, uint16_t cmd_size,
                    uint8_t *rsp, uint16_t *p_rsp_size );
bool p2p_ndef_set_record( uint8_t *p_record, uint16_t record_size,
                         void *cb );
void p2p_ndef_register_pull_callback( void *p_cb );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /*File_H_*/
