/******************************************************************************

 @file  sensortag.h

 @brief This file contains the SensorTag application's definitions and
        prototypes.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2015-2016, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: ble_sdk_2_02_01_18
 Release Date: 2016-10-26 15:20:04
 *****************************************************************************/

#ifndef SENSORTAG_H
#define SENSORTAG_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "ICall.h"
#include "peripheral.h"
#include <ti/sysbios/knl/Clock.h>
#include <ti/drivers/PIN.h>

/*********************************************************************
 * CONSTANTS
 */

// Service ID's for internal application use
#define SERVICE_ID_TMP       0x01
#define SERVICE_ID_OPT       0x02
#define SERVICE_ID_MOV       0x04
#define SERVICE_ID_HUM       0x05
#define SERVICE_ID_BAR       0x06
#define SERVICE_ID_IO        0x07
#define SERVICE_ID_KEYS      0x08
#define SERVICE_ID_CC        0x09
#define SERVICE_ID_DISPLAY   0x0A
#define SERVICE_ID_LIGHT     0x0B
#define SERVICE_ID_REGISTER  0x0C
#define SERVICE_ID_BATT      0x0D
//added for proxreporter markel
#define SERVICE_ID_PROX      0x0E

/*********************************************************************
 * MACROS
 */
// Internal events for RTOS application
#define ST_ICALL_EVT                         ICALL_MSG_EVENT_ID  // Event_Id_31
#define ST_QUEUE_EVT                         UTIL_QUEUE_EVENT_ID // Event_Id_30
#define ST_STATE_CHANGE_EVT                  Event_Id_00         // Add
#define ST_CHAR_CHANGE_EVT                   Event_Id_01         // Add
#define ST_PERIODIC_EVT                      Event_Id_02         // Add
#define ST_BATTERY_CHECK_EVT                 Event_Id_03         // Add
#define SK_KEY_CHANGE_EVT                    Event_Id_04         // Add
#define SK_EVT_FACTORY_RESET                 Event_Id_05         // Add
#define SK_EVT_DISCONNECT                    Event_Id_06         // Add
#define ST_ACCEL_CHANGE_EVT                  Event_Id_07         // Add from keyfob
#define ST_ACCEL_READ_EVT                    Event_Id_08         // Add from keyfob
#define ST_PROXIMITY_EVT                     Event_Id_09         // Add from keyfob
#define ST_TOGGLE_BUZZER_EVT                 Event_Id_10         // Add from keyfob
#define ST_CONN_EVT_END_EVT                  Event_Id_30         // Add

#define ST_ALL_EVENTS                        (ST_ICALL_EVT                 | \
                                              ST_QUEUE_EVT                 | \
                                              ST_STATE_CHANGE_EVT          | \
                                              ST_CHAR_CHANGE_EVT           | \
                                              ST_PERIODIC_EVT              | \
                                              SK_KEY_CHANGE_EVT            | \
                                              SK_EVT_FACTORY_RESET         | \
                                              SK_EVT_DISCONNECT            | \
                                              ST_ACCEL_CHANGE_EVT          | \
                                              ST_ACCEL_READ_EVT            | \
                                              ST_PROXIMITY_EVT             | \
                                              ST_TOGGLE_BUZZER_EVT         | \
                                              ST_CONN_EVT_END_EVT)

// sensortagAlertState values from Key Fob
#define ALERT_STATE_OFF                       0
#define ALERT_STATE_LOW                       1
#define ALERT_STATE_HIGH                      2
// from keyfob end

/*********************************************************************
 * VARIABLES
 */
//extern ICall_Semaphore sem;
extern ICall_SyncHandle syncEvent;
extern gaprole_States_t gapProfileState;
extern ICall_EntityID selfEntityMain;
extern PIN_State pinGpioState;
extern PIN_Handle hGpioPin;
extern uint32_t events;
extern uint8_t sensortagAlertState;


/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task creation function for SensorTag
 */
extern void SensorTag_createTask(void);

/*
 * Function to call when a characteristic value has changed
 */
extern void SensorTag_charValueChangeCB(uint8_t sensorID, uint8_t paramID);

/*
 * Function to load the factory image and reboot it
 */
extern void SensorTag_applyFactoryImage(void);

/*
 * Update the advertising data with the latest key press status
 */
extern void SensorTag_updateAdvertisingData(uint8_t keyStatus);

/*
 * Return the self-test result
 */
extern uint8_t SensorTag_testResult(void);

// stop alert
extern void SensorTag_stopAlert(void);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORTAG_H */
