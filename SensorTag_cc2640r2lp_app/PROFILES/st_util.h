/******************************************************************************

 @file  st_util.h

 @brief Utilities for Sensor Tag services

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2012-2016, Texas Instruments Incorporated
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

#ifndef ST_UTIL_H
#define ST_UTIL_H

/*********************************************************************
 * MACROS
 */
#include "bcomdef.h"
#include "gatt.h"

/*********************************************************************
 * MACROS
 */
#ifdef GATT_TI_UUID_128_BIT

// TI Base 128-bit UUID: F000XXXX-0451-4000-B000-000000000000
#define TI_UUID_SIZE        ATT_UUID_SIZE
#define TI_UUID(uuid)       TI_BASE_UUID_128(uuid)

#else

// Using 16-bit UUID
#define TI_UUID_SIZE        ATT_BT_UUID_SIZE
#define TI_UUID(uuid)       LO_UINT16(uuid), HI_UINT16(uuid)

#endif

// Utility string macros
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Profile Parameter Identifiers
#define SENSOR_DATA                     0  
#define SENSOR_CONF                     1  
#define SENSOR_PERI                     2  

// Data readout periods (range 100 - 2550 ms)
#define SENSOR_MIN_UPDATE_PERIOD        100     // Minimum 100 milliseconds
#define SENSOR_PERIOD_RESOLUTION        10      // Resolution 10 milliseconds

// Common values for turning a sensor on and off + config/status
#define ST_CFG_SENSOR_DISABLE           0x00
#define ST_CFG_SENSOR_ENABLE            0x01
#define ST_CFG_ERROR                    0xFF

/*********************************************************************
 * Profile Callbacks
 */

// Callback when a characteristic value has changed
typedef void (*sensorChange_t)(uint8_t paramID);

typedef struct
{
  sensorChange_t pfnSensorChange;  // Called when characteristic value changes
} sensorCBs_t;

/*-------------------------------------------------------------------
 * FUNCTIONS
 */

extern bStatus_t utilExtractUuid16(gattAttribute_t *pAttr, uint16_t *pValue);

#endif /* ST_UTIL_H */

