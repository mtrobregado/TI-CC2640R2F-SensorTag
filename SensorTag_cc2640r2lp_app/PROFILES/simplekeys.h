/******************************************************************************

 @file  simplekeys.h

 @brief This file contains the Simple Keys Profile header file.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2010-2016, Texas Instruments Incorporated
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

#ifndef SIMPLEKEYS_H
#define SIMPLEKEYS_H

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
#define SK_KEY_ATTR                   0  // RW uint8 - Profile Attribute value
 
// SK Service UUID
#define SK_SERV_UUID                  0xFFE0
    
// Key Pressed UUID
#define SK_KEYPRESSED_UUID            0xFFE1

// Key Values
#define SK_KEY_LEFT                   0x01
#define SK_KEY_RIGHT                  0x02

// Simple Keys Profile Services bit fields
#define SK_SERVICE                    0x00000001

/*********************************************************************
 * TYPEDEFS
 */
  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

/*********************************************************************
 * API FUNCTIONS 
 */

/*
 * SK_AddService - Initializes the Simple Key service by registering
 *                 GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */

extern bStatus_t SK_AddService(uint32 services);
  
/*
 * SK_SetParameter - Set a Simple Key Profile parameter.
 *
 *    param  - Profile parameter ID
 *    len    - length of data to right
 *    pValue - pointer to data to write.  This is dependent on
 *             the parameter ID and WILL be cast to the appropriate 
 *             data type (example: data type of uint16 will be cast to 
 *             uint16 pointer).
 */
extern bStatus_t SK_SetParameter(uint8 param, uint8 len, void *pValue);
  
/*
 * SK_GetParameter - Get a Simple Key Profile parameter.
 *
 *    param  - Profile parameter ID
 *    pValue - pointer to data to write.  This is dependent on
 *             the parameter ID and WILL be cast to the appropriate 
 *             data type (example: data type of uint16 will be cast to 
 *             uint16 pointer).
 */
extern bStatus_t SK_GetParameter(uint8 param, void *pValue);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SIMPLEKEYS_H */
