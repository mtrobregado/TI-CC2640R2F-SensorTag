/******************************************************************************

 @file  sensortag_io.h

 @brief This file contains the Sensor Tag sample application,
        Input/Output control.

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

 TI CC2640R2F Sensortag using TI CC2640R2F Launchpad + Educational BoosterPack MKII

 Maker/Author - Markel T. Robregado

 Modification Details : CC2640R2F Launchpad with SensorTag and Key Fob codes
                        ported from BLE Stack 2.2.1.

 Device Setup: TI CC2640R2F Launchpad + Educational BoosterPack MKII
 *****************************************************************************/


#ifndef SENSORTAG_IO_H
#define SENSORTAG_IO_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "sensortag.h"
#include "board.h"


/*********************************************************************
 * CONSTANTS
 */
// Consistent LED usage between SensorTag and Launchpad
#if defined(__CC2650_LAUNCHXL_BOARD_H__) \
    || defined(__CC1350_LAUNCHXL_BOARD_H__) \
    || defined(__CC2640R2_LAUNCHXL_BOARD_H__)
#define IOID_GREEN_LED          Board_GLED
#define IOID_RED_LED            Board_RLED
#endif

#if defined (__SENSORTAG_H__)
#define IOID_GREEN_LED          BP_GLED
#define IOID_RED_LED            BP_BLED
#endif


#if defined(__CC2650STK_SENSORTAG_BOARD_H__)
#define IOID_GREEN_LED          Board_STK_LED2
#define IOID_RED_LED            Board_STK_LED1
#endif

#if defined(__CC1350STK_SENSORTAG_BOARD_H__)
#define IOID_RED_LED            Board_STK_LED1
#endif

            /*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
#ifndef EXCLUDE_IO
/*
 * Initialize IO module
 */
extern void SensorTagIO_init(void);

/*
 * Task Event Processor for IO module
 */
extern void SensorTagIO_processCharChangeEvt(uint8_t paramID);

/*
 * Reset IO module
 */
extern void SensorTagIO_reset(void);

/*
 * Function to blink LEDs 'n' times
 */
extern void SensorTagIO_blinkLed(uint8_t led, uint8_t nBlinks);

#else

/* IO module not included */

#define SensorTagIO_init()
#define SensorTagIO_reset()
#define SensorTagIO_processCharChangeEvt(paramID)
#define SensorTagIO_blinkLed(led,nBlinks)

#endif // EXCLUDE_IO

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SENSORTAGIO_H */
