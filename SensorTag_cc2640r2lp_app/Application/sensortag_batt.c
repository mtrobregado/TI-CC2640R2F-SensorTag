/******************************************************************************

 @file  sensortag_batt.c

 @brief This file contains the Sensor Tag sample application,
        Battery monitoring sub-task.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2016, Texas Instruments Incorporated
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
#ifndef EXCLUDE_BATT

/*********************************************************************
 * INCLUDES
 */
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

#include "gatt.h"
#include "gattservapp.h"
#include "util.h"

#include "battservice.h"
#include "sensortag_batt.h"
#include "sensortag.h"
#include "st_util.h"

#include "icall_api.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// How often to check battery (milliseconds)
#define BATT_PERIOD         15000

// Battery level is critical when it is less than this %
#define BATT_CRITICAL_LEVEL 60

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
static Clock_Struct periodicClock;
static bool sensorReadScheduled;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void sensorConfigChangeCB(uint8_t event);
static void SensorTagBatt_clockHandler(UArg arg);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */


/*********************************************************************
 * @fn      SensorTagBatt_init
 *
 * @brief   Initialize scheduler for battery monitoring
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagBatt_init(void)
{
  // Add battery service.
  Batt_AddService();

  // Register for Battery service callback.
  Batt_Register(&sensorConfigChangeCB);

  // Initialize the module state variables
  SensorTagBatt_reset();

  // Create periodic clock for internal battery check event
  Util_constructClock(&periodicClock, SensorTagBatt_clockHandler,
                      100, BATT_PERIOD, false, ST_BATTERY_CHECK_EVT);
}

/*********************************************************************
 * @fn      SensorTagBatt_processCharChangeEvt
 *
 * @brief   SensorTag battery monitor event handling
 *
 * @param   event - event identifier
 *
 */
void SensorTagBatt_processCharChangeEvt(uint8_t event)
{
    if (event == BATT_LEVEL_NOTI_ENABLED)
    {
        Util_startClock(&periodicClock);
    }
    else if (event == BATT_LEVEL_NOTI_DISABLED)
    {
        Util_stopClock(&periodicClock);
    }
}


/*********************************************************************
 * @fn      SensorTagBatt_reset
 *
 * @brief   Reset characteristics and disable sensor
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagBatt_reset (void)
{
    // Setup Battery Characteristic Values.
    uint8_t critical = BATT_CRITICAL_LEVEL;
    Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8_t), &critical);

    // Clear any scheduled read
    sensorReadScheduled = false;

    // Make sure clock stops
    Util_stopClock(&periodicClock);
}


/*********************************************************************
* Private functions
*/


/*********************************************************************
 * @fn      SensorTagBatt_processSensorEvent
 *
 * @brief   SensorTag batery monitor event processor.
 *
 */
void SensorTagBatt_processSensorEvent(void)  //nead to always call
{
    if (sensorReadScheduled)
    {
        sensorReadScheduled = false;

        // Perform battery level check.
        Batt_MeasLevel();
    }
}


/*********************************************************************
 * @fn      SensorTagBatt_clockHandler
 *
 * @brief   Handler function for clock time-outs.
 *
 * @param   arg - event type
 *
 * @return  none
 */
static void SensorTagBatt_clockHandler(UArg arg)
{
    sensorReadScheduled = true;

    // Wake up the application.
    Event_post(syncEvent, arg); // Add
}


/*********************************************************************
 * @fn      sensorConfigChangeCB
 *
 * @brief   Callback from Battery Service indicating a configuration change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void sensorConfigChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_BATT, paramID);
}
#endif // EXCLUDE_BATT

/*********************************************************************
*********************************************************************/
