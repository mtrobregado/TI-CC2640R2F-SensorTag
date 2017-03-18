/******************************************************************************

 @file  bma250.c

 @brief This file contains the BMA250 accelerator API implementation for
        CC26xx.

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 ******************************************************************************
 
 Copyright (c) 2011-2016, Texas Instruments Incorporated
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


/******************************************************************************
 * INCLUDES
 */
#include "bma250.h"

/******************************************************************************
 * DEFINES
 */

/******************************************************************************
 * FUNCTION PROTOTYPES
 */

/******************************************************************************
 * LOCAL VARIABLES
 */
static uint8_t acc_initialized = FALSE;

/******************************************************************************
 * FUNCTIONS
 */

/****************************************************************************
* @fn       Acc_init(void)
*
* @brief    Initialize SPI interface and BMA250 accelerometer.
*
* @param    None.
*
* @return   void
****************************************************************************/
void Acc_init(void)
{
  acc_initialized = TRUE;
}

/****************************************************************************
* @fn       Acc_stop(void)
*
* @brief    Sets the BMA250 accelerometer in low-power mode.
*
* @param    None.
*
* @return   void
******************************************************************************/
void Acc_stop(void)
{
  if (acc_initialized)
  {
    // We cheat and simply turn off power to the accelerometer
    acc_initialized = FALSE;
  }
}

/****************************************************************************
* @fn       Acc_writeReg
*
* @brief    Write one byte to a sensor register
*
* @param    reg     Register address
* @param    val     Value to write
*
* @return   void
****************************************************************************/
void Acc_writeReg(uint8_t reg, uint8_t val)
{
  // To be implemented.
}

/****************************************************************************
* @fn       Acc_readReg
*
* @brief    Read one byte from a sensor register
*
* @param    reg     Register address
* @param    val     Pointer to destination of read value
*
* @return   void
****************************************************************************/
void Acc_readReg(uint8_t reg, uint8_t *pVal)
{
  // To be implemented.
}

/****************************************************************************
* @fn       Acc_readAcc
*
* @brief    Read x, y and z acceleration data in one operation.
*
* @param    pXVal   Pointer to destination of read out X acceleration
* @param    pYVal   Pointer to destination of read out Y acceleration
* @param    pZVal   Pointer to destination of read out Z acceleration
*
* @return   void
****************************************************************************/
void Acc_readAcc(int8_t *pXVal, int8_t *pYVal, int8_t *pZVal)
{
  int8_t readout[6] = {0,0,0,0,0,0};
  uint8_t i;

  // Read all data from accelerometer
  for(i = 0; i<6; i++)
  {
    // To be implemented.
  }

  // Use only most significant byte of each channel.
  *pXVal = readout[1];
  *pYVal = readout[3];
  *pZVal = readout[5];
}

/****************************************************************************
* @fn       Acc_readAcc
*
* @brief    Read x, y and z acceleration data in one operation.
*
* @param    pXVal   Pointer to destination of read out X acceleration
* @param    pYVal   Pointer to destination of read out Y acceleration
* @param    pZVal   Pointer to destination of read out Z acceleration
*
* @return   void
****************************************************************************/
void Acc_readAcc16(int16_t *pXVal, int16_t *pYVal, int16_t *pZVal)
{
  int8_t readout[6] = {0,0,0,0,0,0};
  uint8_t i;
  // Read all data from accelerometer
  for(i = 0; i<6; i++)
  {
    // To be implemented.
  }

  // Merge high byte (8b) and low bits (2b) into 16b signed destination
  *pXVal = ((((uint8_t)readout[0]) >> 6) | ((int16_t)(readout[1]) << 2));
  *pYVal = ((((uint8_t)readout[2]) >> 6) | ((int16_t)(readout[3]) << 2));
  *pZVal = ((((uint8_t)readout[4]) >> 6) | ((int16_t)(readout[5]) << 2));
}


/*********************************************************************
*********************************************************************/
