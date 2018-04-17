/******************************************************************************
* Includes
*******************************************************************************/

#include <stddef.h>
#include "RW_NDEF.h"
#include "RW_NDEF_T1T.h"
#include "RW_NDEF_T2T.h"
#include "RW_NDEF_T3T.h"
#include "RW_NDEF_T4T.h"
#include "nfc.h"

#ifdef RW_SUPPORT

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/


/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
typedef void rw_ndef_read_fct_t( uint8_t *p_cmd, uint16_t cmd_size,
                                 uint8_t *rsp, uint16_t *p_rsp_size );

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
/* Allocate buffer for NDEF operations */
uint8_t ndef_buffer[ RW_MAX_NDEF_FILE_SIZE ];

static rw_ndef_read_fct_t *p_read_fct = NULL;

rw_ndef_callback_t *p_rw_ndef_pull_cb;

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
void rw_ndef_register_pull_callback
(
                void *p_cb
)
{
    p_rw_ndef_pull_cb = ( rw_ndef_callback_t * )p_cb;
}

void rw_ndef_reset
(
                uint8_t type
)
{
    switch ( type )
    {
    case RW_NDEF_TYPE_T1T:
        rw_ndef_t1t_reset();
        p_read_fct = rw_ndef_t1t_read_next;
        break;
    case RW_NDEF_TYPE_T2T:
        rw_ndef_t2t_reset();
        p_read_fct = rw_ndef_t2t_read_next;
        break;
    case RW_NDEF_TYPE_T3T:
        rw_ndef_t3t_reset();
        p_read_fct = rw_ndef_t3t_read_next;
        break;
    case RW_NDEF_TYPE_T4T:
        rw_ndef_t4t_reset();
        p_read_fct = rw_ndef_t4t_read_next;
        break;
    default:
        p_read_fct = NULL;
        break;
    }
}

void rw_ndef_read_next
(
                uint8_t *p_cmd,
                uint16_t cmd_size,
        uint8_t *rsp,
                uint16_t *p_rsp_size
)
{
    if( p_read_fct != NULL )
        p_read_fct( p_cmd, cmd_size, rsp, p_rsp_size );
}
#endif


/*************** END OF FUNCTIONS ***************************************************************************/