/******************************************************************************

 @file  sensortag_conn_ctrl.c

 @brief This file contains the Sensor Tag sample application,
        Connection Control, for use with the TI Bluetooth Low
        Energy Protocol Stack.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2014-2016, Texas Instruments Incorporated
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

 TI CC2640R2F Sensortag using TI CC2640R2F Launchpad + Educational BoosterPack MKII

 Maker/Author - Markel T. Robregado

 Modification Details : CC2640R2F Launchpad with SensorTag and Key Fob codes
                        ported from BLE Stack 2.2.1.

 Device Setup: TI CC2640R2F Launchpad + Educational BoosterPack MKII
 *****************************************************************************/

#ifndef EXCLUDE_OAD

/*******************************************************************************
 * INCLUDES
 */

#include <ti/sysbios/knl/Semaphore.h>

#include "gatt.h"
#include "gattservapp.h"
#include "sensortag_conn_ctrl.h"
#include "ccservice.h"

#include "board.h"
#include "peripheral.h"

#include "icall_api.h"

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * EXTERNAL VARIABLES
 */

/*******************************************************************************
 * EXTERNAL FUNCTIONS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void ccChangeCB(uint8_t newParamID);

/*******************************************************************************
 * PROFILE CALLBACKS
 */
static ccCBs_t sensorTag_ccCBs =
{
  ccChangeCB,               // Characteristic value change callback
};


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

/*******************************************************************************
 * @fn      SensorTagConnectionControl_init
 *
 * @brief   Initialization function for the SensorTag keys
 *
 */
void SensorTagConnectionControl_init(void)
{
  // Add service
  CcService_addService();
  CcService_registerAppCBs(&sensorTag_ccCBs);
}

/*******************************************************************************
 * @fn      SensorTagConnectionControl_update
 *
 * @brief   Update the Connection Control service with the current connection
 *          control settings
 *
 */
void SensorTagConnectionControl_update(void)
{
  uint8_t buf[CCSERVICE_CHAR1_LEN];
  uint16_t connInterval;
  uint16_t connSlaveLatency;
  uint16_t connTimeout;

  // Get the connection control data
  GAPRole_GetParameter(GAPROLE_CONN_INTERVAL, &connInterval);
  GAPRole_GetParameter(GAPROLE_SLAVE_LATENCY, &connSlaveLatency);
  GAPRole_GetParameter(GAPROLE_CONN_TIMEOUT, &connTimeout);

  buf[0] = LO_UINT16(connInterval);
  buf[1] = HI_UINT16(connInterval);
  buf[2] = LO_UINT16(connSlaveLatency);
  buf[3] = HI_UINT16(connSlaveLatency);
  buf[4] = LO_UINT16(connTimeout);
  buf[5] = HI_UINT16(connTimeout);

  CcService_setParameter(CCSERVICE_CHAR1, sizeof(buf), buf);
}

/*******************************************************************************
 * @fn      SensorTagConnControl_processCharChangeEvt
 *
 * @brief   Process a change in the connection control characteristics
 *
 * @return  none
 */
void SensorTagConnControl_processCharChangeEvt(uint8_t paramID)
{
  // CCSERVICE_CHAR1: read & notify only

  // CCSERVICE_CHAR: requested connection parameters
  if (paramID == CCSERVICE_CHAR2)
  {
    uint8_t buf[CCSERVICE_CHAR2_LEN];

    // Get new connection parameters
    CcService_getParameter(CCSERVICE_CHAR2, buf);

    // Update connection parameters
    GAPRole_SendUpdateParam(BUILD_UINT16(buf[0],buf[1]),
                            BUILD_UINT16(buf[2],buf[3]), // minConnInterval, maxConnInterval
                            BUILD_UINT16(buf[4],buf[5]),
                            BUILD_UINT16(buf[6],buf[7]), // slaveLatency, timeoutMultiplier
                            GAPROLE_TERMINATE_LINK);
  }
  // CCSERVICE_CHAR3: Disconnect request
  else if (paramID == CCSERVICE_CHAR3)
  {
    // Any change in the value will terminate the connection
    GAPRole_TerminateConnection();
  }
}

/*******************************************************************************
 * @fn      SensorTagConnControl_paramUpdateCB
 *
 * @brief   Called when connection parameters are updates
 *
 * @param   connInterval - new connection interval
 *
 * @param   connSlaveLatency - new slave latency
 *
 * @param   connTimeout - new connection timeout
 *
 * @return  none
*/
void SensorTagConnControl_paramUpdateCB(uint16_t connInterval,
                      uint16_t connSlaveLatency, uint16_t connTimeout)
{
  uint8_t buf[CCSERVICE_CHAR1_LEN];

  buf[0] = LO_UINT16(connInterval);
  buf[1] = HI_UINT16(connInterval);
  buf[2] = LO_UINT16(connSlaveLatency);
  buf[3] = HI_UINT16(connSlaveLatency);
  buf[4] = LO_UINT16(connTimeout);
  buf[5] = HI_UINT16(connTimeout);

  CcService_setParameter(CCSERVICE_CHAR1,sizeof(buf),buf);
}

/*******************************************************************************
* Private functions
*/

/*******************************************************************************
 * @fn      ccChangeCB
 *
 * @brief   Callback from Connection Control indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void ccChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_CC, paramID);
}
#endif // #ifndef EXCLUDE_OAD

/*******************************************************************************
*******************************************************************************/
