#ifndef T4T_NDEF_EMU_H_
#define T4T_NDEF_EMU_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief t4t_ndef_emu_reset
 */
void t4t_ndef_emu_reset( void );

/**
 * @brief t4t_ndef_emu_next
 * @param p_cmd
 * @param cmd_size
 * @param rsp
 * @param p_rsp_size
 */
void t4t_ndef_emu_next( uint8_t *p_cmd, uint16_t cmd_size, 
                        uint8_t *rsp, uint16_t *p_rsp_size );
/**
 * @brief t4t_ndef_emu_set_record
 * @param p_record
 * @param record_size
 * @param cb
 * @return
 */
bool t4t_ndef_emu_set_record( uint8_t *p_record, uint16_t record_size, void *cb );

#ifdef __cplusplus
} // extern "C"
#endif

#endif
