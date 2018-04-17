#ifndef RW_NDEF_T3T_H_
#define RW_NDEF_T3T_H_

/******************************************************************************
* Includes
*******************************************************************************/
#include <stdint.h>


/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief rw_ndef_t3t_reset
 */
void rw_ndef_t3t_reset( void );

/**
 * @brief rw_ndef_t3t_set_id_m
 * @param p_id_m
 */
void rw_ndef_t3t_set_id_m( uint8_t *p_id_m );

/**
 * @brief rw_ndef_t3t_read_next
 * @param p_cmd
 * @param cmd_size
 * @param rsp
 * @param p_rsp_size
 */
void rw_ndef_t3t_read_next( uint8_t *p_cmd, uint16_t cmd_size,
                            uint8_t *rsp, uint16_t *p_rsp_size );

#ifdef __cplusplus
} // extern "C"
#endif

#endif /*File_H_*/

/*** End of File **************************************************************/

