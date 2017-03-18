/******************************************************************************

 @file  gapgattserver.h

 @brief This file contains GAP GATT attribute definitions and prototypes
        prototypes.

 Group: WCS, BTS
 Target Device: CC2640R2

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
 Release Name: ti-ble-3.0-stack-sdk_3_00_00
 Release Date: 2016-12-21 12:44:47
 *****************************************************************************/

#ifndef GAPGATTSERVER_H
#define GAPGATTSERVER_H

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

#define GAP_DEVICE_NAME_LEN                     21 // Excluding null-terminate char

// Privacy Flag States
#define GAP_PRIVACY_DISABLED                    0x00
#define GAP_PRIVACY_ENABLED                     0x01    

// GAP GATT Server Parameters used with GGS Get/Set Parameter and Application's Callback functions
#define GGS_DEVICE_NAME_ATT                     0   // RW  uint8[GAP_DEVICE_NAME_LEN]
#define GGS_APPEARANCE_ATT                      1   // RW  uint16
#define GGS_PERI_PRIVACY_FLAG_ATT               2   // RW  uint8
#define GGS_RECONNCT_ADDR_ATT                   3   // RW  uint8[B_ADDR_LEN]
#define GGS_PERI_CONN_PARAM_ATT                 4   // RW  sizeof(gapPeriConnectParams_t)
#define GGS_PERI_PRIVACY_FLAG_PROPS             5   // RW  uint8
#define GGS_W_PERMIT_DEVICE_NAME_ATT            6   // W   uint8
#define GGS_W_PERMIT_APPEARANCE_ATT             7   // W   uint8
#define GGS_W_PERMIT_PRIVACY_FLAG_ATT           8   // W   uint8
#define GGS_CENT_ADDR_RES_ATT                   9   // RW  uint8

// GAP Services bit fields
#define GAP_SERVICE                             0x00000001

#if defined ( TESTMODES )
  // GGS TestModes
  #define GGS_TESTMODE_OFF                      0 // No Test mode
  #define GGS_TESTMODE_W_PERMIT_DEVICE_NAME     1 // Make Device Name attribute writable
  #define GGS_TESTMODE_W_PERMIT_APPEARANCE      2 // Make Appearance attribute writable
  #define GGS_TESTMODE_W_PERMIT_PRIVACY_FLAG    3 // Make Peripheral Privacy Flag attribute writable with authentication
#endif  // TESTMODES

/*********************************************************************
 * TYPEDEFS
 */
// Callback to notify when attribute value is changed over the air.
typedef void (*ggsAttrValueChange_t)( uint16 connHandle, uint8 attrId );

// GAP GATT Server callback structure
typedef struct
{
  ggsAttrValueChange_t  pfnAttrValueChange;  // When attribute value is changed OTA
} ggsAppCBs_t;

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * Profile Callbacks
 */

/*********************************************************************
 * API FUNCTIONS
 */

/**
 * @brief   Set a GAP GATT Server parameter.
 *
 * @param   param - Profile parameter ID<BR>
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).<BR>
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_SetParameter( uint8 param, uint8 len, void *value );

/**
 * @brief   Get a GAP GATT Server parameter.
 *
 * @param   param - Profile parameter ID<BR>
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).<BR>
 *
 * @return  bStatus_t
 */
extern bStatus_t GGS_GetParameter( uint8 param, void *value );

/**
 * @brief   Add function for the GAP GATT Service.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service added successfully.<BR>
 *          INVALIDPARAMETER: Invalid service field.<BR>
 *          FAILURE: Not enough attribute handles available.<BR>
 *          bleMemAllocError: Memory allocation error occurred.<BR>
 */
extern bStatus_t GGS_AddService( uint32 services );

/**
 * @brief   Delete function for the GAP GATT Service.
 *
 * @param   services - services to delete. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  SUCCESS: Service deleted successfully.<BR>
 *          FAILURE: Service not found.<BR>
 */
extern bStatus_t GGS_DelService( uint32 services );

/**
 * @brief   Registers the application callback function.
 *
 *          Note: Callback registration is needed only when the
 *                Device Name is made writable. The application
 *                will be notified when the Device Name is changed
 *                over the air.
 *
 * @param   appCallbacks - pointer to application callbacks.
 *
 * @return  none
 */
extern void GGS_RegisterAppCBs( ggsAppCBs_t *appCallbacks );

/**
 * @brief   Set a GGS Parameter value. Use this function to change
 *          the default GGS parameter values.
 *
 * @param   value - new GGS param value
 *
 * @return  void
 */
extern void GGS_SetParamValue( uint16 value );

/**
 * @brief   Get a GGS Parameter value.
 *
 * @param   none
 *
 * @return  GGS Parameter value
 */
extern uint16 GGS_GetParamValue( void );

/*********************************************************************
 * TASK FUNCTIONS - Don't call these. These are system functions.
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* GAPGATTSERVER_H */
