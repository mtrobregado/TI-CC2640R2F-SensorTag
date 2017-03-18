/******************************************************************************

 @file  sensortag_keys.h

 @brief This file contains the Sensor Tag sample application,
        Keys part, for use with the TI Bluetooth Low
        Energy Protocol Stack.

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

#ifndef SENSORTAGKEYS_H
#define SENSORTAGKEYS_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "sensortag.h"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

#ifndef EXCLUDE_KEYS
/*
 * Initialize Key module
 */
extern void SensorTagKeys_init(void);

/*
 * Task Event Processor for Key module
 */
extern void SensorTagKeys_processEvent(void);

/*
 * Reset Key module
 */
extern void SensorTagKeys_reset(void);

/*
 * Process right key
 */
extern void SensorTagKeys_processKeyRight(void);

/*
 * Process left key
 */
extern void SensorTagKeys_processKeyLeft(void);

/*
 * Process reed relay
 */
extern void SensorTagKeys_processRelay(void);

#else

/* Keys module not included */
#define SensorTagKeys_init()
#define SensorTagKeys_processEvent()
#define SensorTagKeys_reset()
#define SensorTagKeys_processKeyRight()
#define SensorTagKeys_processKeyLeft()
#define SensorTagKeys_processRelay()

#endif // EXCLUDE_KEYS

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORTAGKEYS_H */
