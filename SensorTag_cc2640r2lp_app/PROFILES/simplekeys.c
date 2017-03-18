/******************************************************************************

 @file  simplekeys.c

 @brief Simple Keys Profile for CC26xx.

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

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "simplekeys.h"

#include "icall_api.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        5

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// SK Service UUID: 0x1800
CONST uint8 skServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SK_SERV_UUID), HI_UINT16(SK_SERV_UUID)
};

// Key Pressed UUID: 0x1801
CONST uint8 keyPressedUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(SK_KEYPRESSED_UUID), HI_UINT16(SK_KEYPRESSED_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * Profile Attributes - variables
 */

// SK Service attribute
static CONST gattAttrType_t skService = { ATT_BT_UUID_SIZE, skServUUID };

// Keys Pressed Characteristic Properties
static uint8 skCharProps = GATT_PROP_NOTIFY;

// Key Pressed State Characteristic
static uint8 skKeyPressed = 0;

// Key Pressed Characteristic Configs
static gattCharCfg_t *skConfig;

// Key Pressed Characteristic User Description
static uint8 skCharUserDesp[16] = "Key Press State";


/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t simplekeysAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Simple Keys Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&skService                     /* pValue */
  },

    // Characteristic Declaration for Keys
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &skCharProps 
    },

      // Characteristic Value- Key Pressed
      { 
        { ATT_BT_UUID_SIZE, keyPressedUUID },
        0, 
        0, 
        &skKeyPressed 
      },

      // Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&skConfig 
      },

      // Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        skCharUserDesp 
      },      
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t SK_readAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, 
                               uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                               uint16_t maxLen, uint8_t method);
static bStatus_t SK_writeAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                uint8_t *pValue, uint16_t len, uint16_t offset,
                                uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// SK Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t skCBs =
{
  SK_readAttrCB,  // Read callback function pointer
  SK_writeAttrCB, // Write callback function pointer
  NULL            // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SK_AddService
 *
 * @brief   Initializes the Simple Key service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t SK_AddService(uint32 services)
{
  uint8 status = SUCCESS;

  // Allocate Client Characteristic Configuration table
  skConfig = (gattCharCfg_t *)ICall_malloc(sizeof(gattCharCfg_t) *
                                            linkDBNumConns);
  if (skConfig == NULL)
  {
    return (bleMemAllocError);
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, skConfig); 
  
  if (services & SK_SERVICE)
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(simplekeysAttrTbl, 
                                         GATT_NUM_ATTRS(simplekeysAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &skCBs);
  }

  return (status);
}

/*********************************************************************
 * @fn      SK_SetParameter
 *
 * @brief   Set a Simple Key Profile parameter.
 *
 * @param   param  - Profile parameter ID
 * @param   len    - length of data to write
 * @param   pValue - pointer to data to write.  This is dependent on
 *                   the parameter ID and WILL be cast to the appropriate 
 *                   data type (example: data type of uint16 will be cast to 
 *                   uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SK_SetParameter(uint8 param, uint8 len, void *pValue)
{
  bStatus_t ret = SUCCESS;
  switch (param)
  {
    case SK_KEY_ATTR:
      if (len == sizeof(uint8)) 
      {
        skKeyPressed = *((uint8*)pValue);
        
        // See if Notification/Indication has been enabled
        GATTServApp_ProcessCharCfg(skConfig, &skKeyPressed, FALSE, 
                                   simplekeysAttrTbl, 
                                   GATT_NUM_ATTRS(simplekeysAttrTbl),
                                   INVALID_TASK_ID, SK_readAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return (ret);
}

/*********************************************************************
 * @fn      SK_GetParameter
 *
 * @brief   Get a Simple Key Profile parameter.
 *
 * @param   param  - Profile parameter ID
 * @param   pValue - Pointer to data to put.  This is dependent on
 *                   the parameter ID and WILL be cast to the appropriate 
 *                   data type (example: data type of uint16 will be cast to 
 *                   uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SK_GetParameter(uint8 param, void *pValue)
{
  bStatus_t ret = SUCCESS;
  switch (param)
  {
    case SK_KEY_ATTR:
      *((uint8*)pValue) = skKeyPressed;
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return (ret);
}

/*********************************************************************
 * @fn          SK_readAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr      - pointer to attribute
 * @param       pValue     - pointer to data to be read
 * @param       pLen       - length of data to be read
 * @param       offset     - offset of the first octet to be read
 * @param       maxLen     - maximum length of data to be read
 * @param       method     - type of read message 
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t SK_readAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, 
                               uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                               uint16_t maxLen, uint8_t method)
{
  bStatus_t status = SUCCESS;
 
  // Make sure it's not a blob operation (no attributes in the profile are long
  if (offset > 0)
  {
    return (ATT_ERR_ATTR_NOT_LONG);
  }
 
  if (pAttr->type.len == ATT_BT_UUID_SIZE)
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch (uuid)
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles this type for reads

      // simple keys characteristic does not have read permissions, but because it
      //   can be sent as a notification, it must be included here
      case SK_KEYPRESSED_UUID:
        *pLen = 1;
        pValue[0] = *pAttr->pValue;
        break;

      default:
        // Should never get here!
        *pLen = 0;
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return (status);
}

/*********************************************************************
 * @fn      SK_writeAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr      - pointer to attribute
 * @param   pValue     - pointer to data to be written
 * @param   len        - length of data
 * @param   offset     - offset of the first octet to be written
 * @param   method     - type of write message 
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t SK_writeAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                uint8_t *pValue, uint16_t len, uint16_t offset,
                                uint8_t method)
{
  bStatus_t status = SUCCESS;

  if (pAttr->type.len == ATT_BT_UUID_SIZE)
  {
    // 16-bit UUID
    uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch (uuid)
    {
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                                offset, GATT_CLIENT_CFG_NOTIFY);
        break;
       
      default:
        // Should never get here!
        status = ATT_ERR_ATTR_NOT_FOUND;
        break;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  return (status);
}

/*********************************************************************
*********************************************************************/
