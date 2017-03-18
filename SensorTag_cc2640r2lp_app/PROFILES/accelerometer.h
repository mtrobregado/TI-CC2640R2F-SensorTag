/******************************************************************************

 @file  accelerometer.h

 @brief This file contains Accelerometer Profile header file.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2009-2016, Texas Instruments Incorporated
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

#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Profile Parameters
#define ACCEL_ENABLER                 0  // RW uint8 - Profile Attribute value
#define ACCEL_X_ATTR                  1  // RW int16 - Profile Attribute value
#define ACCEL_Y_ATTR                  2  // RW int16 - Profile Attribute value
#define ACCEL_Z_ATTR                  3  // RW int16 - Profile Attribute value
#define ACCEL_RANGE                   4  // RW uint16 - Profile Attribute value
  
// Profile UUIDs
#define ACCEL_ENABLER_UUID            0xFFA1
#define ACCEL_RANGE_UUID              0xFFA2
#define ACCEL_X_UUID                  0xFFA3
#define ACCEL_Y_UUID                  0xFFA4
#define ACCEL_Z_UUID                  0xFFA5
  
// Accelerometer Service UUID
#define ACCEL_SERVICE_UUID            0xFFA0
  
// Profile Range Values
#define ACCEL_RANGE_2G                20
#define ACCEL_RANGE_8G                80

// Accelerometer Profile Services bit fields
#define ACCEL_SERVICE                 0x00000001

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */
// Callback when the device has been started.  Callback event to 
// the ask for a battery check.
typedef void (*accelEnabler_t)(void);

typedef struct
{
  accelEnabler_t     pfnAccelEnabler;  // Called when Enabler attribute changes
} accelCBs_t;

/*********************************************************************
 * API FUNCTIONS 
 */

/*
 * Accel_AddService - Initializes the Accelerometer service by registering 
 *          GATT attributes with the GATT server. Only call this function once.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t Accel_AddService(uint32 services);

/*
 * Accel_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Accel_RegisterAppCBs(accelCBs_t *appCallbacks);


/*
 * Accel_SetParameter - Set an Accelerometer Profile parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Accel_SetParameter(uint8 param, uint8 len, void *value);
  
/*
 * Accel_GetParameter - Get an Accelerometer Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t Accel_GetParameter(uint8 param, void *value);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ACCELEROMETER_H */
