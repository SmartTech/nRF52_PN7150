/*******************************************************************************
* Title                 :   System Initialization
* Filename              :   sys_init.c
* Author                :   JWB
* Origin Date           :   04/23/2012
* Notes                 :   None
*******************************************************************************/
/*************** MODULE REVISION LOG ******************************************
*
*    Date    Software Version    Initials   Description
*  XX/XX/XX    XXXXXXXXXXX         JWB      Module Created.
*
*******************************************************************************/
/** @file XXX.c
 *  @brief This module contains the
 */
/******************************************************************************
* Includes
*******************************************************************************/

#ifndef NFC_DEMO_H
#define NFC_DEMO_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "nfc.h"

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef CARDEMU_SUPPORT
#include "T4T_NDEF_EMU.h"
#endif
#ifdef P2P_SUPPORT

#include "P2P_NDEF.h"
#endif
#ifdef RW_SUPPORT
#include "RW_NDEF.h"
#endif

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/
#if defined P2P_SUPPORT || defined RW_SUPPORT

#define NDEF_PRINT_URI_CODE(x) {                                      \
            switch(x)                                                 \
            {                                                         \
                case 1: NFC_LOG_INFO("http://www."); break;           \
                case 2: NFC_LOG_INFO("https://www."); break;          \
                case 3: NFC_LOG_INFO("http://"); break;               \
                case 4: NFC_LOG_INFO("https://"); break;              \
                case 5: NFC_LOG_INFO("tel:"); break;                  \
                case 6: NFC_LOG_INFO("mailto:"); break;               \
                default: break;                                       \
             }                                                        \
             NFC_LOG_FLUSH();                                         \
}
#endif

#if defined P2P_SUPPORT || defined RW_SUPPORT
char *auth(uint8_t x);
char *encrypt(uint8_t x);
void ndef_pull_cb(uint8_t *p_ndef_record, uint16_t ndef_record_size);
#endif

#if defined P2P_SUPPORT || defined CARDEMU_SUPPORT
void ndef_push_cb(uint8_t *p_ndef_record, uint16_t ndef_record_size);
#endif

void mi_fare_scenario();

void process_radio(nfc_interface_t *radio);

int nfc_demo_init();

#endif