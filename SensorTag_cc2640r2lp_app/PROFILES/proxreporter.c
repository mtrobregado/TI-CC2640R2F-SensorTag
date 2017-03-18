/******************************************************************************

 @file  proxreporter.c

 @brief Proximity Profile - Reporter Role

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

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gatt_profile_uuid.h"
#include "gattservapp.h"
#include "gapbondmgr.h"

#include "proxreporter.h"

#include "icall_api.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define PP_DEFAULT_TX_POWER               0
#define PP_DEFAULT_PATH_LOSS              0x7F

#define SERVAPP_NUM_ATTR_SUPPORTED        5

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Link Loss Service UUID.
CONST uint8 linkLossServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(LINK_LOSS_SERV_UUID), HI_UINT16(LINK_LOSS_SERV_UUID)
};

// Immediate Alert Service UUID.
CONST uint8 imAlertServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(IMMEDIATE_ALERT_SERV_UUID), HI_UINT16(IMMEDIATE_ALERT_SERV_UUID)
};

// Tx Power Level Service UUID.
CONST uint8 txPwrLevelServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(TX_PWR_LEVEL_SERV_UUID), HI_UINT16(TX_PWR_LEVEL_SERV_UUID)
};

// Alert Level Attribute UUID.
CONST uint8 alertLevelUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ALERT_LEVEL_UUID), HI_UINT16(ALERT_LEVEL_UUID)
};

// Tx Power Level Attribute UUID.
CONST uint8 txPwrLevelUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(TX_PWR_LEVEL_UUID), HI_UINT16(TX_PWR_LEVEL_UUID)
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
static proxReporterCBs_t *pr_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Link Loss Service.
static CONST gattAttrType_t linkLossService = { ATT_BT_UUID_SIZE, 
                                                linkLossServUUID };

// Alert Level Characteristic Properties.
static uint8 llAlertLevelCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Alert Level attribute.
// This attribute enumerates the level of alert.
static uint8 llAlertLevel = PP_ALERT_LEVEL_NO;

// Immediate Alert Service.
static CONST gattAttrType_t imAlertService = { ATT_BT_UUID_SIZE, 
                                               imAlertServUUID };

// Alert Level Characteristic Properties.
static uint8 imAlertLevelCharProps = GATT_PROP_WRITE_NO_RSP;

// Alert Level attribute.
// This attribute enumerates the level of alert.
static uint8 imAlertLevel = PP_ALERT_LEVEL_NO;

// Tx Power Level Service.
static CONST gattAttrType_t txPwrLevelService = { ATT_BT_UUID_SIZE, 
                                                  txPwrLevelServUUID };

// Tx Power Level Characteristic Properties.
static uint8 txPwrLevelCharProps = GATT_PROP_READ | GATT_PROP_NOTIFY;

// Tx Power Level attribute.
// This attribute represents the range of transmit power levels in dBm with
// a range from -20 to +20 to a resolution of 1 dBm.
static int8 txPwrLevel = PP_DEFAULT_TX_POWER;

// Tx Power Level Characteristic Configs
static gattCharCfg_t *txPwrLevelConfig;


/*********************************************************************
 * Profile Attributes - Table
 */
// Link Loss Service Atttribute Table.
static gattAttribute_t linkLossAttrTbl[] = 
{
  // Link Loss service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&linkLossService               /* pValue */
  },

    // Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &llAlertLevelCharProps 
    },

      // Alert Level attribute
      { 
        { ATT_BT_UUID_SIZE, alertLevelUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        &llAlertLevel 
      },
};

// Immediate Alert Service Atttribute Table.
static gattAttribute_t imAlertAttrTbl[] = 
{
  // Immediate Alert service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&imAlertService                /* pValue */
  },

    // Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &imAlertLevelCharProps 
    },

      // Alert Level attribute
      { 
        { ATT_BT_UUID_SIZE, alertLevelUUID },
        GATT_PERMIT_WRITE, 
        0, 
        &imAlertLevel 
      },
};

// Tx Power Level Service Atttribute Table.
static gattAttribute_t txPwrLevelAttrTbl[] = 
{
  // Tx Power Level service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&txPwrLevelService             /* pValue */
  },

    // Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &txPwrLevelCharProps 
    },

      // Tx Power Level attribute
      { 
        { ATT_BT_UUID_SIZE, txPwrLevelUUID },
        GATT_PERMIT_READ, 
        0, 
        (uint8 *)&txPwrLevel 
      },

      // Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&txPwrLevelConfig 
      },

};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t ProxReporter_ReadAttrCB(uint16_t connHandle, 
                                         gattAttribute_t *pAttr,
                                         uint8_t *pValue, uint16_t *pLen,
                                         uint16_t offset, uint16_t maxLen,
                                         uint8_t method);

static bStatus_t ProxReporter_WriteAttrCB(uint16_t connHandle, 
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t len,
                                          uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Prox Reporter Service Callbacks.
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t proxReporterCBs =
{
  ProxReporter_ReadAttrCB,  // Read callback function pointer
  ProxReporter_WriteAttrCB, // Write callback function pointer
  NULL                      // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      ProxReporter_AddService
 *
 * @brief   Initializes the Proximity Reporter service by
 *          registering GATT attributes with the GATT server.
 *          Only call this function once.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return   Success or Failure
 */
bStatus_t ProxReporter_AddService(uint32 services)
{
  uint8 status = SUCCESS;

  if (services & PP_LINK_LOSS_SERVICE)
  {
    // Register Link Loss attribute list and CBs with GATT Server App.
    status = GATTServApp_RegisterService(linkLossAttrTbl, 
                                         GATT_NUM_ATTRS(linkLossAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &proxReporterCBs);
  }

  if ((status == SUCCESS) && (services & PP_IM_ALETR_SERVICE))
  {
    // Register Link Loss attribute list and CBs with GATT Server App.
    status = GATTServApp_RegisterService(imAlertAttrTbl, 
                                         GATT_NUM_ATTRS(imAlertAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &proxReporterCBs);
  }
  
  if ((status == SUCCESS)  && (services & PP_TX_PWR_LEVEL_SERVICE))
  {
    // Allocate Client Characteristic Configuration table
    txPwrLevelConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                      linkDBNumConns );
    if ( txPwrLevelConfig != NULL )
    {   
      // Initialize Client Characteristic Configuration attributes.
      GATTServApp_InitCharCfg(INVALID_CONNHANDLE, txPwrLevelConfig); 
      
      // Register Tx Power Level attribute list and CBs with GATT Server App.
      status = GATTServApp_RegisterService(txPwrLevelAttrTbl, 
                                           GATT_NUM_ATTRS(txPwrLevelAttrTbl),
                                           GATT_MAX_ENCRYPT_KEY_SIZE,
                                           &proxReporterCBs);
    }
    else
    {
      status = bleMemAllocError;
    }
  }

  return (status);
}

/*********************************************************************
 * @fn      ProxReporter_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t ProxReporter_RegisterAppCBs(proxReporterCBs_t *appCallbacks)
{
  if (appCallbacks)
  {
    pr_AppCBs = appCallbacks;
    
    return (SUCCESS);
  }
  else
  {
    return (bleAlreadyInRequestedMode);
  }
} 

/*********************************************************************
 * @fn      ProxReporter_SetParameter
 *
 * @brief   Set a Proximity Reporter parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ProxReporter_SetParameter(uint8 param, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;
  
  switch (param)
  {
    case PP_LINK_LOSS_ALERT_LEVEL:
      if ((len == sizeof (uint8)) && 
          ((*((uint8*)value) <= PP_ALERT_LEVEL_HIGH))) 
      {
        llAlertLevel = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case PP_IM_ALERT_LEVEL:
      if (len == sizeof (uint8)) 
      {
        imAlertLevel = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case PP_TX_POWER_LEVEL:
      if (len == sizeof (int8)) 
      {
        txPwrLevel = *((int8*)value);

        // See if notifications have been enabled.
        GATTServApp_ProcessCharCfg(txPwrLevelConfig, (uint8 *)&txPwrLevel, 
                                   FALSE, txPwrLevelAttrTbl, 
                                   GATT_NUM_ATTRS(txPwrLevelAttrTbl),
                                   INVALID_TASK_ID, ProxReporter_ReadAttrCB);
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
 * @fn      ProxReporter_GetParameter
 *
 * @brief   Get a Proximity Reporter parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t ProxReporter_GetParameter(uint8 param, void *value)
{
  bStatus_t ret = SUCCESS;
  switch (param)
  {
    case PP_LINK_LOSS_ALERT_LEVEL:
      *((uint8*)value) = llAlertLevel;
      break;
      
    case PP_IM_ALERT_LEVEL:
      *((uint8*)value) = imAlertLevel;
      break;
      
    case PP_TX_POWER_LEVEL:
      *((int8*)value) = txPwrLevel;
      break;
 
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return (ret);
}

/*********************************************************************
 * @fn          ProxReporter_ReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message 
 *
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t ProxReporter_ReadAttrCB(uint16_t connHandle, 
                                         gattAttribute_t *pAttr,
                                         uint8_t *pValue, uint16_t *pLen,
                                         uint16_t offset, uint16_t maxLen,
                                         uint8_t method)
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation.
  if (offset > 0)
  {
    return (ATT_ERR_ATTR_NOT_LONG);
  }  

  if (pAttr->type.len == ATT_BT_UUID_SIZE)
  {
    // 16-bit UUID.
    uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
    
    switch (uuid)
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those types for reads.
      case ALERT_LEVEL_UUID:
      case TX_PWR_LEVEL_UUID:
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
    //128-bit UUID.
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return (status);
}

/*********************************************************************
 * @fn      ProxReporter_WriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message 
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t ProxReporter_WriteAttrCB(uint16_t connHandle, 
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t len,
                                          uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notify = 0xFF;

  if (pAttr->type.len == ATT_BT_UUID_SIZE)
  { 
    // 16-bit UUID.
    uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch (uuid)
    { 
      case ALERT_LEVEL_UUID:
        // Validate the value.
        // Make sure it's not a blob operation.
        if (offset == 0)
        {
          if (len > 1)
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
          else
          {
            if (pValue[0] > PP_ALERT_LEVEL_HIGH)
            {
              status = ATT_ERR_INVALID_VALUE;
            }
          }
        }
        else
        {
          status = ATT_ERR_ATTR_NOT_LONG;
        }
        
        // Write the value.
        if (status == SUCCESS)
        {
          uint8 *pCurValue = (uint8 *)pAttr->pValue;
          
          *pCurValue = pValue[0];
          if(pAttr->pValue == &llAlertLevel)
          {
            notify = PP_LINK_LOSS_ALERT_LEVEL;        
          }
          else
          {
            notify = PP_IM_ALERT_LEVEL;                      
          }
        }
        
        break;

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
    // 128-bit UUID.
    status = ATT_ERR_INVALID_HANDLE;
  }    
  
  // If an attribute changed then callback function to notify application of 
  // change.
  if ((notify != 0xFF) && pr_AppCBs && pr_AppCBs->pfnAttrChange)
  {
    pr_AppCBs->pfnAttrChange(notify);
  }

  return (status);
}


/*********************************************************************
*********************************************************************/
