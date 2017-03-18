/******************************************************************************

 @file  sensortag_io.c

 @brief This file contains the Sensor Tag sample application,
        Input/Output control.

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
 *****************************************************************************/

#ifndef EXCLUDE_IO
/*********************************************************************
 * INCLUDES
 */

#include <ti/sysbios/knl/Event.h>

#include "gatt.h"
#include "gattservapp.h"
#include "sensortag_io.h"
#include "ioservice.h"
#include "sensortag_buzzer.h"
#ifdef FACTORY_IMAGE
#include "sensortag_factoryreset.h"
#include "ExtFlash.h"
#endif

#include "board.h"
#include "peripheral.h"
#include "util.h"
#include "SensorMpu9250.h"
#include "SensorUtil.h"

#include "icall_api.h" //Add

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define IO_DATA_LED1            0x01 // Red
#define IO_DATA_LED2            0x02 // Green
#define IO_DATA_BUZZER          0x04

#ifdef FACTORY_IMAGE
#define IO_DATA_EXT_FLASH_ERASE 0x08
#endif

#define BLINK_DURATION          20   // Milliseconds

#ifdef Board_BUZZER
#define BUZZER_FREQUENCY        2000
#endif

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
static uint8_t ioMode;
static uint8_t ioValue;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void ioChangeCB(uint8_t newParamID);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static sensorCBs_t sensorTag_ioCBs =
{
  ioChangeCB,               // Characteristic value change callback
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SensorTagIO_init
 *
 * @brief   Initialization function for the SensorTag IO
 *
 * @parama  none
 *
 * @return  none
 */
void SensorTagIO_init(void)
{
  // Add service
  Io_addService();
  Io_registerAppCBs(&sensorTag_ioCBs);

  // Initialize the module's state variables
  ioMode = IO_MODE_LOCAL;
  ioValue = 0;

  // Set internal state
  SensorTagIO_reset();
}


/*********************************************************************
 * @fn      SensorTagIO_processCharChangeEvt
 *
 * @brief   Process a change in the IO characteristics
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagIO_processCharChangeEvt(uint8_t paramID)
{
  if (paramID == SENSOR_CONF)
  {

    Io_getParameter(SENSOR_CONF, &ioMode);
    if (ioMode == IO_MODE_SELFTEST)
    {
      ioValue = SensorTag_testResult();
      Io_setParameter(SENSOR_DATA, 1, &ioValue);
    }
    else
    {
      // Mode change: make sure LEDs and buzzer are off
      Io_setParameter(SENSOR_DATA, 1, &ioValue);

      PIN_setOutputValue(hGpioPin, IOID_RED_LED, Board_LED_OFF);
#ifdef IOID_GREEN_LED
      PIN_setOutputValue(hGpioPin, IOID_GREEN_LED, Board_LED_OFF);
#endif
#ifdef Board_BUZZER
      SensorTagBuzzer_close();
#endif
    }
  }
  else if (paramID == SENSOR_DATA)
  {
    Io_getParameter(SENSOR_DATA, &ioValue);
  }

  if (ioMode == IO_MODE_REMOTE)
  {
    // Control by remote client:
    // - possible to operate the LEDs and buzzer
    // - right key functionality overridden (will not terminate connection)
    if (!!(ioValue & IO_DATA_LED1))
    {
      PIN_setOutputValue(hGpioPin, IOID_RED_LED, Board_LED_ON);
    }
    else
    {
      PIN_setOutputValue(hGpioPin, IOID_RED_LED, Board_LED_OFF);
    }
#ifdef IOID_GREEN_LED
    if (!!(ioValue & IO_DATA_LED2))
    {
      PIN_setOutputValue(hGpioPin, IOID_GREEN_LED, Board_LED_ON);
    }
    else
    {
      PIN_setOutputValue(hGpioPin, IOID_GREEN_LED, Board_LED_OFF);
    }
#endif

#ifdef Board_BUZZER
    if (!!((ioValue & IO_DATA_BUZZER)))
    {
      // Start buzzer (PWM)
      SensorTagBuzzer_open(hGpioPin);
      SensorTagBuzzer_setFrequency(BUZZER_FREQUENCY);
    }
    else
    {
      SensorTagBuzzer_close();
    }
#endif
#ifdef FACTORY_IMAGE
    if (!!((ioValue & IO_DATA_EXT_FLASH_ERASE)))
    {
        SensorTagFactoryReset_extFlashErase();
    }
#endif
  }
}

/*********************************************************************
 * @fn      SensorTagIO_reset
 *
 * @brief   Reset characteristics
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagIO_reset(void)
{
  ioValue = SensorTag_testResult();
  Io_setParameter(SENSOR_DATA, 1, &ioValue);

  ioMode = IO_MODE_LOCAL;
  Io_setParameter(SENSOR_CONF, 1, &ioMode);

  // Normal mode; make sure LEDs and buzzer are off
  PIN_setOutputValue(hGpioPin, IOID_RED_LED, Board_LED_OFF);
#ifdef IOID_GREEN_LED
  PIN_setOutputValue(hGpioPin, IOID_GREEN_LED, Board_LED_OFF);
#endif

#ifdef Board_BUZZER
  SensorTagBuzzer_close();
#endif
}


/*********************************************************************
 * @fn      SensorTagIO_GetIoMode
 *
 * @brief   Get the current IO mode
 *
 * @return  IO mode (IO_MODE_SELFTEST, IO_MODE_REMOTE, IO_MODE_LOCAL)
 */
uint8_t SensorTagIO_GetMode(void)
{
  return ioMode;
}

/*******************************************************************************
 * @fn      SensorTagIO_blinkLed
 *
 * @brief   Blinks a led 'n' times, duty-cycle 50-50
 * @param   led - led identifier
 * @param   nBlinks - number of blinks
 *
 * @return  none
 */
void SensorTagIO_blinkLed(uint8_t led, uint8_t nBlinks)
{
  uint8_t i;

  for (i = 0; i<nBlinks; i++)
  {
    PIN_setOutputValue(hGpioPin, led, Board_LED_ON);
    DELAY_MS(BLINK_DURATION);
    PIN_setOutputValue(hGpioPin, led, Board_LED_OFF);
    DELAY_MS(BLINK_DURATION);
  }
}

/*********************************************************************
* Private functions
*/

/*********************************************************************
 * @fn      ioChangeCB
 *
 * @brief   Callback from IO service indicating a value change
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
static void ioChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_IO, paramID);
}
#endif // EXCLUDE_IO

/*********************************************************************
*********************************************************************/

