/******************************************************************************

 @file  sensortag_batt.h

 @brief This file contains the Sensor Tag sample application, battery
        monitoring sub-task.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2016, Texas Instruments Incorporated
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

#ifndef SENSORTAGBATT_H
#define SENSORTAGBATT_H

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
#ifndef EXCLUDE_BATT

/*
 * Initialization Battery module
 */
extern void SensorTagBatt_init(void);

/*
 * Task Event Processor for characteristic changes
 */
extern void SensorTagBatt_processCharChangeEvt(uint8_t paramID);

/*
 * Task Event Processor for Sensor reading
 */
extern void SensorTagBatt_processSensorEvent(void);

/*
 * Reset Battery module
 */
extern void SensorTagBatt_reset(void);

#else

/* Battery module not included */

#define SensorTagBatt_init()
#define SensorTagBatt_processCharChangeEvt(paramID)
#define SensorTagBatt_processSensorEvent()
#define SensorTagBatt_reset()

#endif // EXCLUDE_BATT

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORTAGBATT_H */
