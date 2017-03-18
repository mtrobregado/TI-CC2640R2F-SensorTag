/******************************************************************************

 @file  ioservice.c

 @brief IO Service.

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

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "linkdb.h"
#include "att.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "gattservapp.h"
#include "string.h"

#include "ioservice.h"
#include "st_util.h"

#include "icall_api.h"


/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// GATT Profile Service UUID
CONST uint8_t ioServUUID[TI_UUID_SIZE] =
{
  TI_UUID(IO_SERV_UUID)
};

// Data Characteristic UUID
CONST uint8_t ioDataUUID[TI_UUID_SIZE] =
{
  TI_UUID(IO_DATA_UUID)
};

// Config Characteristic UUID
CONST uint8_t ioConfUUID[TI_UUID_SIZE] =
{
  TI_UUID(IO_CONF_UUID)
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

static sensorCBs_t *io_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// IO Service attribute
static CONST gattAttrType_t ioService = { TI_UUID_SIZE, ioServUUID };

// IO Service Data Characteristic Properties
static uint8_t ioDataProps = GATT_PROP_READ | GATT_PROP_WRITE;

// IO Service Data Characteristic Value
static uint8_t ioData = 0;

// IO Characteristic Configuration
static gattCharCfg_t *ioDataConfig;

#ifdef USER_DESCRIPTION
// IO Service Data Characteristic User Description
static uint8_t ioDataUserDesp[] = "IO Data";
#endif

// IO Service Config Characteristic Properties
static uint8_t ioConfProps = GATT_PROP_READ | GATT_PROP_WRITE;

// IO Service Config Characteristic Value
static uint8_t ioConf = 0x00;

#ifdef USER_DESCRIPTION
// IO Service Config Characteristic User Description
static uint8_t ioConfUserDesp[] = "IO Config";
#endif

/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t ioAttrTbl[] =
{
  // IO Service Service
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8_t *)&ioService                     /* pValue */
  },

    // Data Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ioDataProps
    },

      // Data Characteristic Value
      {
        { TI_UUID_SIZE, ioDataUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &ioData
      },
#ifdef USER_DESCRIPTION
      // Data Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        ioDataUserDesp
      },
#endif
    // Config Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &ioConfProps
    },

      // Config Characteristic Value
      {
        { TI_UUID_SIZE, ioConfUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &ioConf
      },
#ifdef USER_DESCRIPTION
      // Config Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        ioConfUserDesp
      },
#endif
};


/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t io_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                               uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                               uint16_t maxLen, uint8_t method);
static bStatus_t io_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                uint8_t *pValue, uint16_t len, uint16_t offset,
                                uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// IO Service Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t ioCBs =
{
  io_ReadAttrCB,  // Read callback function pointer
  io_WriteAttrCB, // Write callback function pointer
  NULL            // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Io_addService
 *
 * @brief   Initializes the IO Service service by registering
 *          GATT attributes with the GATT server.
 *
 * @return  Success or Failure
 */
bStatus_t Io_addService(void)
{
  // Allocate Client Characteristic Configuration table
  ioDataConfig = (gattCharCfg_t *)ICall_malloc(sizeof(gattCharCfg_t) *
                                                linkDBNumConns);
  if (ioDataConfig == NULL)
  {
    return (bleMemAllocError);
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg(INVALID_CONNHANDLE, ioDataConfig);

  // Register GATT attribute list and CBs with GATT Server App
  return GATTServApp_RegisterService(ioAttrTbl,
                                      GATT_NUM_ATTRS(ioAttrTbl),
                                      GATT_MAX_ENCRYPT_KEY_SIZE,
                                      &ioCBs);
}

/*********************************************************************
 * @fn      Io_registerAppCBs
 *
 * @brief   Registers the application callback function. Only call
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t Io_registerAppCBs(sensorCBs_t *appCallbacks)
{
  if (appCallbacks != NULL)
  {
    io_AppCBs = appCallbacks;
    
    return (SUCCESS);
  }
  else
  {
    return (bleAlreadyInRequestedMode);
  }
}

/*********************************************************************
 * @fn      Io_setParameter
 *
 * @brief   Set a IO Service parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Io_setParameter(uint8_t param, uint8_t len, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (param)
  {
    case SENSOR_DATA:
      if (len == sizeof(uint8_t))
      {
        ioData = *((uint8_t*)value);
        // See if Notification has been enabled
        ret = GATTServApp_ProcessCharCfg(ioDataConfig, &ioData, FALSE,
                                   ioAttrTbl, GATT_NUM_ATTRS(ioAttrTbl),
                                   INVALID_TASK_ID, io_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SENSOR_CONF:
      if(len == sizeof(uint8_t))
      {
        ioConf = *((uint8_t*)value);
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
 * @fn      Io_getParameter
 *
 * @brief   Get a IO Service parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16_t will be cast to
 *          uint16_t pointer).
 *
 * @return  bStatus_t
 */
bStatus_t Io_getParameter(uint8_t param, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (param)
  {
    case SENSOR_DATA:
      *((uint8_t*)value) = ioData;
      break;

    case SENSOR_CONF:
      *((uint8_t*)value) = ioConf;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
}

/*********************************************************************
 * @fn          io_ReadAttrCB
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
static bStatus_t io_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                               uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                               uint16_t maxLen, uint8_t method)
{
  uint16_t uuid;
  bStatus_t status = SUCCESS;

  // Make sure it's not a blob operation (no attributes in the profile are long)
  if (offset > 0)
  {
    return (ATT_ERR_ATTR_NOT_LONG);
  }

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  if (uuid == IO_DATA_UUID || uuid == IO_CONF_UUID)
  {
    *pLen = sizeof(uint8_t);
    pValue[0] = pAttr->pValue[0];
  }
  else
  {
    // Should never get here!
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return (status);
}

/*********************************************************************
 * @fn      io_WriteAttrCB
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
static bStatus_t io_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                uint8_t *pValue, uint16_t len, uint16_t offset,
                                uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8_t notifyApp = 0xFF;
  uint16_t uuid;

  if (utilExtractUuid16(pAttr,&uuid) == FAILURE)
  {
    // Invalid handle
    return ATT_ERR_INVALID_HANDLE;
  }

  switch (uuid)
  {
    case IO_DATA_UUID:
    case IO_CONF_UUID:
      // Validate the value
      // Make sure it's not a blob oper
      if (offset == 0)
      {
        if (len == sizeof(uint8_t))
        {
          if (uuid == IO_CONF_UUID && (pValue[0] >= IO_MODE_NUM_MODES))
            status = ATT_ERR_INVALID_VALUE;
        } else
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if (status == SUCCESS)
      {
        uint8_t *pCurValue = (uint8_t *)pAttr->pValue;
        pCurValue[0] = pValue[0];

        if (uuid == IO_CONF_UUID)
        {
          if (pAttr->pValue == &ioConf)
          {
            notifyApp = SENSOR_CONF;
          }
        }
        else  // uuid == IO_DATA_UUID
        {
          if (pAttr->pValue == &ioData)
          {
            notifyApp = SENSOR_DATA;
          }
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

  // If a characteristic value changed then callback function to 
  // notify application of change
  if ((notifyApp != 0xFF) && io_AppCBs && io_AppCBs->pfnSensorChange)
  {
    io_AppCBs->pfnSensorChange(notifyApp);
  }

  return (status);;
}

/*********************************************************************
*********************************************************************/
