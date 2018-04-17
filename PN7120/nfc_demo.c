/******************************************************************************
* Includes
*******************************************************************************/

#include "nfc_demo.h"
#include "nrf.h"
#include "nrf_delay.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
static nfc_mode_t mode = NFC_MODE_NONE;
volatile bool incoming_flag;

#if defined P2P_SUPPORT || defined CARDEMU_SUPPORT
static char NDEF_RECORD[] = { 0xD1,                                              // MB / ME / CF / 1 / IL / TNF
                             0x01,                                              // TYPE LENGTH
                             51,                                                // PAYLOAD LENTGH
                             'T',                                               // TYPE
                             0x02,                                              // Status
                             'e', 'n',                                          // Language
13, 10, 13, 10, 32, 32, 32, 32, 32, 
'M', 'i', 'k', 'r', 'o', 'E', 'l', 'e', 'k', 't', 'r', 'o', 'n', 'i', 'k', 'a', 
13, 10, 13, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
'N', 'F', 'C', ' ', 'c', 'l', 'i', 'c', 'k'
};
#endif

uint8_t discovery_technologies[] = { MODE_POLL | TECH_PASSIVE_NFCA
                                   , MODE_POLL | TECH_PASSIVE_NFCB
                                   , MODE_POLL | TECH_PASSIVE_NFCF
#ifdef P2P_SUPPORT
                                   , MODE_POLL | TECH_ACTIVE_NFCF
#endif
                                   , MODE_LISTEN | TECH_PASSIVE_NFCA
#ifdef P2P_SUPPORT
                                   , MODE_LISTEN | TECH_PASSIVE_NFCF
                                   , MODE_LISTEN | TECH_ACTIVE_NFCA
                                   , MODE_LISTEN | TECH_ACTIVE_NFCF
#endif
                                   };

/******************************************************************************
* Function Definitions
*******************************************************************************/

#if defined P2P_SUPPORT || defined RW_SUPPORT

char *auth( uint8_t x )
{
    switch( x )
    {
        case 0x01:
            return "Open";
        case 0x02:
            return "WPA-Personal";
        case 0x04:
            return "Shared";
        case 0x08:
            return "WPA-Entreprise";
        case 0x10:
            return "WPA2-Entreprise";
        case 0x20:
            return "WPA2-Personal";
        default:
            return "unknown";
    }
}

char *encrypt( uint8_t x )
{
    switch( x )
    {
        case 0x01:
            return "None";
        case 0x02:
            return "WEP";
        case 0x04:
            return "TKIP";
        case 0x08:
            return "AES";
        case 0x10:
            return "AES/TKIP";
        default:
            return "unknown";
    }
}

void ndef_pull_cb( uint8_t *p_ndef_record, uint16_t ndef_record_size )
{
    char tmp_txt[ 80 ];                                                         /* The callback only print out the received message */
    NFC_LOG_INFO( "--- NDEF Record received:\n" );
    NFC_LOG_FLUSH();

    if( p_ndef_record[ 0 ] == 0xD1 )                                            /* Only short, not fragmented and well-known type records are supported here */
    {
        switch( p_ndef_record[ 3 ] )
        {
            case 'T':
                p_ndef_record[ 7 + p_ndef_record[ 2 ] ] = '\0';
                sprintf( tmp_txt,
                         "   Text record (language = %c%c): %s\n",
                         p_ndef_record[ 5 ],
                         p_ndef_record[ 6 ],
                         &p_ndef_record[ 7 ] );
                NFC_LOG_INFO( tmp_txt );
                break;

            case 'U':
                NFC_LOG_INFO( "   URI record: " );
                NDEF_PRINT_URI_CODE( p_ndef_record[ 4 ] )
                p_ndef_record[ 4 + p_ndef_record[ 2 ] ] = '\0';
                sprintf( tmp_txt, "%s\n", &p_ndef_record[ 5 ] );
                NFC_LOG_INFO( tmp_txt );
                break;

            default:
                NFC_LOG_INFO( "   Unsupported NDEF record, only 'T' and 'U' types are supported\n" );
                break;
        }
        NFC_LOG_FLUSH();
    }                                                                           /* Only short, not fragmented and WIFI handover type are supported here */
    else if( ( p_ndef_record[ 0 ] == 0xD2 ) &&
             ( memcmp( &p_ndef_record[ 3 ], "application/vnd.wfa.wsc",
                       sizeof( "application/vnd.wfa.wsc" ) ) ) )
    {
        uint8_t index = 26, i;

        NFC_LOG_INFO( "--- Received WIFI credentials:\n" );
        NFC_LOG_FLUSH();

        if( ( p_ndef_record[ index ] == 0x10 ) &&
                ( p_ndef_record[ index + 1 ] == 0x0E ) )
            index += 4;

        while( index < ndef_record_size )
        {
            if( p_ndef_record[ index ] == 0x10 )
            {
                if ( p_ndef_record[ index + 1 ] == 0x45 )
                {
                    NFC_LOG_INFO ( "- SSID = " );

                    for( i = 0; i < p_ndef_record[ index + 3 ]; i++ )
                    {
                        sprintf( tmp_txt, "%c", p_ndef_record[ index + 4 + i ] );
                        NFC_LOG_INFO( tmp_txt );
                    }
                    NFC_LOG_INFO ( "\n" );
                    NFC_LOG_FLUSH();
                }
                else if ( p_ndef_record[ index + 1 ] == 0x03 )
                {
                    sprintf( tmp_txt, "- Authenticate Type = %s\n",
                             auth( p_ndef_record[ index + 5 ] ) );
                    NFC_LOG_INFO( tmp_txt );
                    NFC_LOG_FLUSH();
                }
                else if( p_ndef_record[index + 1] == 0x0f )
                {
                    sprintf( tmp_txt, "- Encryption Type = %s\n",
                             encrypt( p_ndef_record[index + 5] ) );
                    NFC_LOG_INFO( tmp_txt );
                    NFC_LOG_FLUSH();
                }
                else if ( p_ndef_record[ index + 1 ] == 0x27 )
                {
                    NFC_LOG_INFO ( "- Network key = " );

                    for( i = 0; i < p_ndef_record[ index + 3 ]; i++ )
                        NFC_LOG_INFO( "#" );
                    NFC_LOG_INFO ( "\n" );
                    NFC_LOG_FLUSH();
                }

                index += 4 + p_ndef_record[ index + 3 ];
            }
            else
                continue;
        }
    }
    else {
        NFC_LOG_INFO( "   Unsupported NDEF record, cannot parse\n" );
        NFC_LOG_FLUSH();
    }

    NFC_LOG_INFO( "\n" );
    NFC_LOG_FLUSH();
}
#endif

#if defined P2P_SUPPORT || defined CARDEMU_SUPPORT
void ndef_push_cb( uint8_t *p_ndef_record, uint16_t ndef_record_size )
{
    NFC_LOG_INFO( "--- NDEF Record sent\n\n" );
    NFC_LOG_FLUSH();
}
#endif


void mi_fare_scenario()
{
    bool status;
    uint8_t i;
    uint8_t resp[256];
    uint8_t resp_size;
    char tmp_txt[ 80 ] = { 0 };
    uint8_t auth[] = { 0x40, 0x01, 0x10, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };  /* Authenticate sector 1 with generic keys */
    uint8_t read[] = { 0x10, 0x30, 0x04 };                                      /* Read block 4 */
    uint8_t write_part1[] = { 0x10, 0xA0, 0x04 };                                                                                                                                                        // Write block 4
    uint8_t write_part2[] = { 0x10, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                    0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
                                                                                /* Authenticate */
    status = nfc_reader_tag_cmd( auth,
                                 sizeof( auth ),
                                 resp,
                                 &resp_size );

    if( ( status == 1 ) || ( resp[ resp_size - 1 ] != 0 ) ){
        sprintf( tmp_txt, " Authenticate sector %d failed with error 0x%02x\n", auth[ 1 ], resp[ resp_size - 1 ] );
        NFC_LOG_INFO( tmp_txt );/* Read block */
        NFC_LOG_FLUSH();
        return;
    }
    sprintf( tmp_txt, " Authenticate sector %d succeed\n", auth[ 1 ] );
    NFC_LOG_INFO( tmp_txt );
    NFC_LOG_FLUSH();
                                                                                /* Read block */
    status = nfc_reader_tag_cmd( read, sizeof( read ), resp, &resp_size );

    if( ( status == 1 ) || ( resp[ resp_size - 1 ] != 0 ) ){
        sprintf( tmp_txt, " Read block %d failed with error 0x%02x\n", read[ 2 ], resp[ resp_size - 1 ] );
        NFC_LOG_INFO( tmp_txt );
        NFC_LOG_FLUSH();
        return;
    }

    sprintf( tmp_txt, " Read block %d: ", read[2] );
    NFC_LOG_INFO( tmp_txt );
    NFC_LOG_FLUSH();

    for( i = 0; i < resp_size - 2; i++ )
    {
        sprintf( tmp_txt, "0x%02X ", resp[i + 1] );
        NFC_LOG_INFO( tmp_txt );
        NFC_LOG_FLUSH();
    }
    NFC_LOG_INFO( "\n" );
                                                                                /* Write block */
    status = nfc_reader_tag_cmd( write_part1, sizeof( write_part1 ), resp, &resp_size );

    if( ( status == 1 ) || ( resp[resp_size - 1] != 0 ) ){
        sprintf( tmp_txt, " Write block %d failed with error 0x%02x\n", write_part1[2], resp[resp_size - 1] );
        NFC_LOG_INFO( tmp_txt );
        NFC_LOG_FLUSH();
        return;
    }

    status = nfc_reader_tag_cmd( write_part2, sizeof( write_part2 ), resp, &resp_size );

    if( ( status == 1 ) || ( resp[resp_size - 1] != 0 ) ){
        sprintf( tmp_txt, " Write block %d failed with error 0x%02x\n", write_part1[2], resp[resp_size - 1] );
        NFC_LOG_INFO( tmp_txt );
        NFC_LOG_FLUSH();
        return;
    }

    sprintf( tmp_txt, " Block %d written\n", write_part1[2] );
    NFC_LOG_INFO( tmp_txt );
    NFC_LOG_FLUSH();
                                                                                /* Read block */
    status = nfc_reader_tag_cmd( read, sizeof( read ), resp, &resp_size );

    if( ( status == 1 ) || ( resp[resp_size - 1] != 0 ) )
    {
        sprintf( tmp_txt, " Read failed with error 0x%02x\n", resp[ resp_size - 1 ] );
        NFC_LOG_INFO( tmp_txt );
        NFC_LOG_FLUSH();
        return;
    }

    sprintf( tmp_txt, " Read block %d: ", read[2] );
    NFC_LOG_INFO( tmp_txt );
    NFC_LOG_FLUSH();

    for( i = 0; i < resp_size - 2; i++ )
    {
        sprintf( tmp_txt, "0x%02X ", resp[i + 1] );
        NFC_LOG_INFO( tmp_txt );
    }
    NFC_LOG_INFO( "\n" );
    NFC_LOG_FLUSH();

    while( 1 )                                                                  /* Perform presence check */
    {
        nrf_delay_ms( 500 );
        status = nfc_reader_tag_cmd( read, sizeof( read ), resp, &resp_size );

        if( ( status == 1 ) || ( resp[resp_size - 1] == 0xb2 ) )
            break;
    }
}

void process_radio
(
                nfc_interface_t *radio
)
{
    char tmp_txt[ 80 ];

#ifdef CARDEMU_SUPPORT

    if( ( radio->interface == INTF_ISODEP ) && ( radio->mode_tech == ( MODE_LISTEN | TECH_PASSIVE_NFCA ) ) ){
        NFC_LOG_INFO( " - LISTEN MODE: Activated from remote Reader\r\n" );
        NFC_LOG_FLUSH();
        nfc_process( NFC_MODE_CARDEMU, radio );
        NFC_LOG_INFO( "READER DISCONNECTED\r\n" );
        NFC_LOG_FLUSH();
    }
    else
#endif
#ifdef P2P_SUPPORT

        if( radio->interface == INTF_NFCDEP ) {
            if( ( radio->mode_tech & MODE_LISTEN ) == MODE_LISTEN )             /* Is target mode ? */
            {
                NFC_LOG_INFO( " - P2P TARGET MODE: Remote Initiator\r\n" );
                NFC_LOG_FLUSH();
            }
            else 
            {
                NFC_LOG_INFO( " - P2P INITIATOR MODE: Remote Target activated\r\n" );
                NFC_LOG_FLUSH();
            }
            nfc_process( NFC_MODE_P2P, radio );
            NFC_LOG_INFO( "PEER LOST\r\n" );
            NFC_LOG_FLUSH();
        }
        else
#endif
#ifdef RW_SUPPORT

            if( ( radio->mode_tech & MODE_MASK ) == MODE_POLL )
            {
                if( radio->protocol == PROT_MIFARE )                            /* Is card detected MIFARE ?*/
                {
                    NFC_LOG_INFO( " - POLL MODE: Remote MIFARE card activated\r\n" );
                    NFC_LOG_FLUSH();
                    mi_fare_scenario();
                    nfc_restart_discovery();                                    /* Restart discovery loop */
                }
                else if( ( radio->protocol != PROT_NFCDEP ) && \
                                 ( radio->interface != INTF_UNDETERMINED ) )    /* Is Undetermined target ?*/
                {
                    sprintf( tmp_txt, " - POLL MODE: Remote T%dT activated\r\n",
                             radio->protocol );
                    NFC_LOG_INFO( tmp_txt );
                    NFC_LOG_FLUSH();
                    nfc_process( NFC_MODE_RW, radio );
                }
                else
                {
                    NFC_LOG_INFO( " - POLL MODE: Undetermined target\r\n" );
                    NFC_LOG_FLUSH();
                    nfc_stop_discovery();                                       /* Restart discovery loop */
                    nfc_start_discovery( discovery_technologies,
                                         sizeof( discovery_technologies ) );
                }

                NFC_LOG_INFO( "CARD DISCONNECTED\r\n" );
                NFC_LOG_FLUSH();
            }
            else
#endif
            {
                NFC_LOG_INFO( "WRONG DISCOVERY\r\n" );
                NFC_LOG_FLUSH();
            }

}

int nfc_demo_init()
{
    mode = NFC_MODE_NONE;
    
#ifdef CARDEMU_SUPPORT
    /* Register NDEF message to be sent to remote reader */
    t4t_ndef_emu_set_record( ( uint8_t * )NDEF_RECORD, sizeof( NDEF_RECORD ),
                             ( void* )ndef_push_cb );
#endif

#ifdef P2P_SUPPORT
    /* Register NDEF message to be sent to remote peer */
    p2p_ndef_set_record( ( uint8_t * )NDEF_RECORD, sizeof( NDEF_RECORD ),
                         ( void* )ndef_push_cb );

    /* Register callback for reception of NDEF message from remote peer */
    p2p_ndef_register_pull_callback( ( void* )ndef_pull_cb );
#endif

#ifdef RW_SUPPORT
    /* Register callback for reception of NDEF message from remote cards */
    rw_ndef_register_pull_callback( ( void* )ndef_pull_cb );                   
#endif

#ifdef CARDEMU_SUPPORT
    /* Set NXPNCI in all modes */
    mode |= NFC_MODE_CARDEMU;                                                   
#endif
    
#ifdef P2P_SUPPORT
    mode |= NFC_MODE_P2P;
#endif
    
#ifdef RW_SUPPORT
    mode |= NFC_MODE_RW;
#endif

    /* Open connection to NXPNCI device */
    if( nfc_init( NFC_I2C_SLAVE ) == NFC_ERROR)                                             
    {
        NFC_LOG_INFO( "Error: cannot connect to NFC click" );
        NFC_LOG_FLUSH();
        return -1;
    }
    else if( nfc_configure( mode ) )
    {
        NFC_LOG_INFO( "Error: cannot configure NFC click" );
        NFC_LOG_FLUSH();
        return -1;
    }
    else if( nfc_start_discovery( discovery_technologies,
                                  sizeof( discovery_technologies ) ) )
    {
        NFC_LOG_INFO( "Error: cannot start discovery" );
        NFC_LOG_FLUSH();
        return -1;
    }

    NFC_LOG_INFO( "NFC Successfully Initialized" );
    NFC_LOG_FLUSH();
    incoming_flag = false;
    return 1;
}