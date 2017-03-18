/******************************************************************************

 @file  proxreporter.h

 @brief This file contains Proximity - Reporter header file.

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

#ifndef PROXIMITYREPORTER_H
#define PROXIMITYREPORTER_H

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
#define PP_LINK_LOSS_ALERT_LEVEL         0  // RW uint8 - Profile Attribute value
#define PP_IM_ALERT_LEVEL                1  // RW uint8 - Profile Attribute value
#define PP_TX_POWER_LEVEL                2  // RW int8 - Profile Attribute value

// Alert Level Values
#define PP_ALERT_LEVEL_NO               0x00
#define PP_ALERT_LEVEL_LOW              0x01
#define PP_ALERT_LEVEL_HIGH             0x02
  
// Proximity Profile Services bit fields
#define PP_LINK_LOSS_SERVICE            0x00000001 // Link Loss Service
#define PP_IM_ALETR_SERVICE             0x00000002 // Immediate Alert Service
#define PP_TX_PWR_LEVEL_SERVICE         0x00000004 // Tx Power Level Service

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
// the Notify of an attribute change.
typedef void (*ppAttrChange_t)(uint8 attrParamID);

typedef struct
{
  ppAttrChange_t        pfnAttrChange;  // Whenever the Link Loss Alert attribute changes
} proxReporterCBs_t;

/*********************************************************************
 * API FUNCTIONS 
 */
 
/*
 * ProxReporter_AddService - Initializes the Proximity Reporter service by
 *                           registering GATT attributes with the GATT server. 
 *                           Only call this function once.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 */
extern bStatus_t ProxReporter_AddService(uint32 services);

/*
 * ProxReporter_RegisterAppCBs - Registers the application callback function.
 *                               Only call this function once.
 *
 * @param   appCallbacks - pointer to application callbacks.
 */
extern bStatus_t ProxReporter_RegisterAppCBs(proxReporterCBs_t *appCallbacks);


/*
 * ProxReporter_SetParameter - Set a Proximity Reporter parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len   - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProxReporter_SetParameter(uint8 param, uint8 len, void *value);
  
/*
 * ProxReporter_GetParameter - Get a Proximity Reporter parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 */
extern bStatus_t ProxReporter_GetParameter(uint8 param, void *value);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* PROXIMITYREPORTER_H */
