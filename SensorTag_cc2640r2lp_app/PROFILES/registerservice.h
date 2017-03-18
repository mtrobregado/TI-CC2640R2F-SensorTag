/******************************************************************************

 @file  registerservice.h

 @brief Generic register access service for I2C/SPI devices

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

#ifndef REGISTERSERVICE_H
#define REGISTERSERVICE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "st_util.h"

/*********************************************************************
 * CONSTANTS
 */

// Service UUID
#define REGISTER_SERV_UUID        0xAC00 // F000AC00-0451-4000-B000-00000000-0000
#define REGISTER_DATA_UUID        0xAC01
#define REGISTER_ADDR_UUID        0xAC02
#define REGISTER_DEV_UUID         0xAC03

// Attribute Identifiers
#define REGISTER_DATA             0
#define REGISTER_ADDRESS          1
#define REGISTER_DEVICE           2

// Attribute sizes
#define REGISTER_DATA_LEN         16
#define REGISTER_ADDRESS_LEN      5 // Byte 0: address length, byte 1-4 addr.
#define REGISTER_DEVICE_LEN       2 // Byte 0: interface, byte: device address

// Interfaces
#define REGISTER_INTERFACE_I2C0   0 // TMP007,BMP280,OPT3001,SHT21
#define REGISTER_INTERFACE_I2C1   1 // MPU9250
#define REGISTER_INTERFACE_MCU    5 // MCU address space
#define REGISTER_INTERFACE_NUM    6 // Number of defined interfaces

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * API FUNCTIONS
 */


/*
 * Register_addService- Initializes the Sensor GATT Profile service by registering
 *          GATT attributes with the GATT server.
 */
extern bStatus_t Register_addService(void);

/*
 * Register_registerAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
extern bStatus_t Register_registerAppCBs(sensorCBs_t *appCallbacks);

/*
 * Register_setParameter - Set a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t Register_setParameter(uint8_t param, uint8_t len, void *value);

/*
 * Register_getParameter - Get a Sensor GATT Profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to read.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 */
extern bStatus_t Register_getParameter(uint8_t param, void *value);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* REGISTERSERVICE_H */
