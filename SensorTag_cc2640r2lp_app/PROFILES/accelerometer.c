/******************************************************************************

 @file  accelerometer.c

 @brief Accelerometer Profile for CC26xx.

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
#include "gattservapp.h"

#include "accelerometer.h"

#include "icall_api.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SERVAPP_NUM_ATTR_SUPPORTED        19

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Accelerometer Service UUID
CONST uint8 accServUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_SERVICE_UUID), HI_UINT16(ACCEL_SERVICE_UUID)
};

// Accelerometer Enabler UUID
CONST uint8 accEnablerUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_ENABLER_UUID), HI_UINT16(ACCEL_ENABLER_UUID)
};

// Accelerometer Range UUID
CONST uint8 rangeUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_RANGE_UUID), HI_UINT16(ACCEL_RANGE_UUID)
};

// Accelerometer X-Axis Data UUID
CONST uint8 xUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_X_UUID), HI_UINT16(ACCEL_X_UUID)
};

// Accelerometer Y-Axis Data UUID
CONST uint8 yUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_Y_UUID), HI_UINT16(ACCEL_Y_UUID)
};

// Accelerometer Z-Axis Data UUID
CONST uint8 zUUID[ATT_BT_UUID_SIZE] =
{ 
  LO_UINT16(ACCEL_Z_UUID), HI_UINT16(ACCEL_Z_UUID)
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
static accelCBs_t *accel_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Accelerometer Service attribute
static CONST gattAttrType_t accelService = { ATT_BT_UUID_SIZE, accServUUID };

// Enabler Characteristic Properties
static uint8 accelEnabledCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Enabler Characteristic Value
static uint8 accelEnabled = FALSE;

// Enabler Characteristic user description
static uint8 accelEnabledUserDesc[14] = "Accel Enable";

// Range Characteristic Properties
static uint8 accelRangeCharProps = GATT_PROP_READ;

// Range Characteristic Value
static uint16 accelRange = ACCEL_RANGE_2G;

// Range Characteristic user description
static uint8 accelRangeUserDesc[13] = "Accel Range";

// Accel Coordinate Characteristic Properties
static uint8 accelXCharProps = GATT_PROP_NOTIFY;
static uint8 accelYCharProps = GATT_PROP_NOTIFY;
static uint8 accelZCharProps = GATT_PROP_NOTIFY;

// Accel Coordinate Characteristics
static int8 accelXCoordinates = 0;
static int8 accelYCoordinates = 0;
static int8 accelZCoordinates = 0;

// Client Characteristic configuration. Each client has its own instantiation
// of the Client Characteristic Configuration. Reads of the Client Characteristic
// Configuration only shows the configuration for that client and writes only
// affect the configuration of that client.

// Accel Coordinate Characteristic Configs
static gattCharCfg_t *accelXConfigCoordinates;
static gattCharCfg_t *accelYConfigCoordinates;
static gattCharCfg_t *accelZConfigCoordinates;

// Accel Coordinate Characteristic user descriptions
static uint8 accelXCharUserDesc[20] = "Accel X-Coordinate";
static uint8 accelYCharUserDesc[20] = "Accel Y-Coordinate";
static uint8 accelZCharUserDesc[20] = "Accel Z-Coordinate";

/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t accelAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] = 
{
  // Accelerometer Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&accelService                  /* pValue */
  },
  
    // Accel Enabler Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelEnabledCharProps 
    },

      // Accelerometer Enable Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, accEnablerUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0,
        &accelEnabled 
      },

      // Accelerometer Enable User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0,
        (uint8*)&accelEnabledUserDesc 
      },

    // Accel Range Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelRangeCharProps 
    },

      // Accelerometer Range Char Value
      { 
        { ATT_BT_UUID_SIZE, rangeUUID },
        GATT_PERMIT_READ, 
        0,
        (uint8*)&accelRange 
      },

      // Accelerometer Range User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0,
        accelRangeUserDesc 
      },
      
    // X-Coordinate Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelXCharProps 
    },
  
      // X-Coordinate Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, xUUID },
        0, 
        0, 
        (uint8 *)&accelXCoordinates
      },
      
      // X-Coordinate Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&accelXConfigCoordinates 
      },

      // X-Coordinate Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        accelXCharUserDesc
      },  

   // Y-Coordinate Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelYCharProps 
    },
  
      // Y-Coordinate Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, yUUID },
        0, 
        0, 
        (uint8 *)&accelYCoordinates 
      },
      
      // Y-Coordinate Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&accelYConfigCoordinates
      },

      // Y-Coordinate Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        accelYCharUserDesc
      },

   // Z-Coordinate Characteristic Declaration
    { 
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ, 
      0,
      &accelZCharProps 
    },
  
      // Z-Coordinate Characteristic Value
      { 
        { ATT_BT_UUID_SIZE, zUUID },
        0, 
        0, 
        (uint8 *)&accelZCoordinates
      },
      
      // Z-Coordinate Characteristic configuration
      { 
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE, 
        0, 
        (uint8 *)&accelZConfigCoordinates 
      },

      // Z-Coordinate Characteristic User Description
      { 
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ, 
        0, 
        accelZCharUserDesc
      },  
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t accel_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, 
                                  uint8_t *pValue, uint16_t *pLen,
                                  uint16_t offset, uint16_t maxLen,
                                  uint8_t method);
static bStatus_t accel_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t len,
                                   uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Accelerometer Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t  accelCBs =
{
  accel_ReadAttrCB,  // Read callback function pointer
  accel_WriteAttrCB, // Write callback function pointer
  NULL               // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Accel_AddService
 *
 * @brief   Initializes the Accelerometer service by
 *          registering GATT attributes with the GATT server. Only
 *          call this function once.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t Accel_AddService(uint32 services)
{
  uint8 status = SUCCESS;
  size_t allocSize = sizeof(gattCharCfg_t) * linkDBNumConns;

  // Allocate Client Characteristic Configuration tables
  accelXConfigCoordinates = (gattCharCfg_t *)ICall_malloc(allocSize);
  if (accelXConfigCoordinates == NULL)
  {     
    return (bleMemAllocError);
  }
  
  accelYConfigCoordinates = (gattCharCfg_t *)ICall_malloc(allocSize);
  if (accelYConfigCoordinates == NULL)
  {
    // Free already allocated data
    ICall_free(accelXConfigCoordinates);
      
    return (bleMemAllocError);
  }
    
  accelZConfigCoordinates = (gattCharCfg_t *)ICall_malloc(allocSize);
  if (accelZConfigCoordinates == NULL)
  {
    // Free already allocated data
    ICall_free(accelXConfigCoordinates);
    ICall_free(accelYConfigCoordinates);
      
    return (bleMemAllocError);
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, accelXConfigCoordinates);
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, accelYConfigCoordinates);
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, accelZConfigCoordinates);

  if (services & ACCEL_SERVICE)
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(accelAttrTbl, 
                                         GATT_NUM_ATTRS(accelAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &accelCBs);
  }

  return (status);
}

/*********************************************************************
 * @fn      Accel_RegisterAppCBs
 *
 * @brief   Does the profile initialization.  Only call this function
 *          once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Accel_RegisterAppCBs(accelCBs_t *appCallbacks)
{
  if (appCallbacks)
  {
    accel_AppCBs = appCallbacks;
    
    return (SUCCESS);
  }
  else
  {
    return (bleAlreadyInRequestedMode);
  }
}

/*********************************************************************
 * @fn      Accel_SetParameter
 *
 * @brief   Set an Accelerometer Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len   - length of data to right
 * @param   value - pointer to data to write.  This is dependent on
 *                  the parameter ID and WILL be cast to the appropriate 
 *                  data type (example: data type of uint16 will be cast to 
 *                  uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Accel_SetParameter(uint8 param, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (param)
  {
    case ACCEL_ENABLER:
      if (len == sizeof (uint8)) 
      {
        accelEnabled = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case ACCEL_RANGE:
      if ((len == sizeof (uint16)) && ((*((uint8*)value)) <= ACCEL_RANGE_8G)) 
      {
        accelRange = *((uint16*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
      
    case ACCEL_X_ATTR:
      if (len == sizeof (int8)) 
      {      
        accelXCoordinates = *((int8*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg(accelXConfigCoordinates, 
                                   (uint8 *)&accelXCoordinates, FALSE, 
                                   accelAttrTbl, GATT_NUM_ATTRS(accelAttrTbl),
                                   INVALID_TASK_ID, accel_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_Y_ATTR:
      if (len == sizeof (int8)) 
      {      
        accelYCoordinates = *((int8*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg(accelYConfigCoordinates, 
                                   (uint8 *)&accelYCoordinates, FALSE,
                                   accelAttrTbl, GATT_NUM_ATTRS(accelAttrTbl),
                                   INVALID_TASK_ID, accel_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_Z_ATTR:
      if (len == sizeof (int8)) 
      {      
        accelZCoordinates = *((int8*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg(accelZConfigCoordinates, 
                                   (uint8 *)&accelZCoordinates, FALSE,
                                   accelAttrTbl, GATT_NUM_ATTRS(accelAttrTbl),
                                   INVALID_TASK_ID, accel_ReadAttrCB);
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
 * @fn      Accel_GetParameter
 *
 * @brief   Get an Accelerometer Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *                  the parameter ID and WILL be cast to the appropriate 
 *                  data type (example: data type of uint16 will be cast to 
 *                  uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Accel_GetParameter(uint8 param, void *value)
{
  bStatus_t ret = SUCCESS;
  switch (param)
  {
    case ACCEL_ENABLER:
      *((uint8*)value) = accelEnabled;
      break;
      
    case ACCEL_RANGE:
      *((uint16*)value) = accelRange;
      break;
      
    case ACCEL_X_ATTR:
      *((int8*)value) = accelXCoordinates;
      break;

    case ACCEL_Y_ATTR:
      *((int8*)value) = accelYCoordinates;
      break;

    case ACCEL_Z_ATTR:
      *((int8*)value) = accelZCoordinates;
      break;
      
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return (ret);
}

/*********************************************************************
 * @fn          accel_ReadAttr
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
static bStatus_t accel_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr, 
                                  uint8_t *pValue, uint16_t *pLen,
                                  uint16_t offset, uint16_t maxLen,
                                  uint8_t method)
{
  uint16 uuid;
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation
  if (offset > 0)
  {
    return (ATT_ERR_ATTR_NOT_LONG);
  }

  if (pAttr->type.len == ATT_BT_UUID_SIZE)
  {    
    // 16-bit UUID
    uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch (uuid)
    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those types for reads
      case ACCEL_RANGE_UUID:
        *pLen = 2;
        pValue[0] = LO_UINT16(*((uint16 *)pAttr->pValue));
        pValue[1] = HI_UINT16(*((uint16 *)pAttr->pValue));
        break;
  
      case ACCEL_ENABLER_UUID:
      case ACCEL_X_UUID:
      case ACCEL_Y_UUID:
      case ACCEL_Z_UUID:
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
 * @fn      accel_WriteAttrCB
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
static bStatus_t accel_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t len,
                                   uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notify = 0xFF;

  if (pAttr->type.len == ATT_BT_UUID_SIZE)
  {
    uint16 uuid = BUILD_UINT16(pAttr->type.uuid[0], pAttr->type.uuid[1]);
    switch (uuid)
    {
      case ACCEL_ENABLER_UUID:
        // Validate the value.
        // Make sure it's not a blob operation.
        if (offset == 0)
        {
          if (len > 1)
          {
            status = ATT_ERR_INVALID_VALUE_SIZE;
          }
          else if (pValue[0] != FALSE && pValue[0] != TRUE)
          {
            status = ATT_ERR_INVALID_VALUE;
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
          notify = ACCEL_ENABLER;        
        }
             
        break;
          
      case GATT_CLIENT_CHAR_CFG_UUID:
        status = GATTServApp_ProcessCCCWriteReq(connHandle, pAttr, pValue, len,
                                                offset, GATT_CLIENT_CFG_NOTIFY);
        break;      
          
      default:
          // Should never get here!
          status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else
  {
    // 128-bit UUID
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If an attribute changed then callback function to notify application of 
  // change.
  if ((notify != 0xFF) && accel_AppCBs && accel_AppCBs->pfnAccelEnabler)
  {
    accel_AppCBs->pfnAccelEnabler();  
  }
  
  return (status);
}


/*********************************************************************
*********************************************************************/
