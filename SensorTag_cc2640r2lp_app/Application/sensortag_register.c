/******************************************************************************

 @file  sensortag_register.c

 @brief This file contains the Sensor Tag sample application,
        Register Service implementation.

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
 *****************************************************************************/

#ifndef EXCLUDE_REG

/*********************************************************************
 * INCLUDES
 */
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include "Board.h"

#include "bcomdef.h"
#include "sensortag_register.h"
#include "registerservice.h"
#include "peripheral.h"
#include "util.h"
#include "string.h"
#ifdef Board_I2C0
#include "SensorI2C.h"
#endif
#ifdef Board_MPU9250_ADDR
#include "SensorMpu9250.h"
#endif

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */
typedef struct
{
    uint32_t readCount;       // Read counter, for debugging only
    uint8_t interfaceID;      // Interface ID (I2C, MCU)
    uint8_t deviceAddress;    // Device address, for I2C only
    uint8_t dataLength;       // 4 bytes for MCU, 1-2 for sensors
    uint8_t reserved1;        // not used
    uint32_t registerAddress; // Internal address offset
    uint8_t data[REGISTER_DATA_LEN];
} RegisterInfo_t;

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
static RegisterInfo_t regInfo;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void registerChangeCB(uint8_t newParamID);
static bool readRegister(uint8_t *pData, bool saveData);
static void writeRegister(uint8_t *pData);

/*********************************************************************
 * PROFILE CALLBACKS
 */
static sensorCBs_t sensorTag_registerCBs =
{
  registerChangeCB,               // Characteristic value change callback
};


/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SensorTagRegister_init
 *
 * @brief   Initialization function for the SensorTag Register Server
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagRegister_init(void)
{
  // Add service
  Register_addService();
  Register_registerAppCBs(&sensorTag_registerCBs);

  // Initialise register to MCU memory space, "this"" structure
  SensorTagRegister_reset();
}


/*********************************************************************
 * @fn      SensorTagRegister_processCharChangeEvt
 *
 * @brief   Process a change in the register characteristics
 *
 * @parama  paramID - identifies the characteristic that has changed
 *
 * @return  none
 */
void SensorTagRegister_processCharChangeEvt(uint8_t paramID)
{
    if (paramID == REGISTER_DATA)
    {
        // Register data is updated; write it to the device
        Register_getParameter(REGISTER_DATA, regInfo.data);
        writeRegister(regInfo.data);
    }
    else if (paramID == REGISTER_ADDRESS)
    {
        uint8_t addr[REGISTER_ADDRESS_LEN];

        // Register address has changed; update register record
        Register_getParameter(REGISTER_ADDRESS, &addr);
        regInfo.dataLength = addr[0];
        memcpy(&regInfo.registerAddress, &addr[1], REGISTER_ADDRESS_LEN - 1);
    }
    else if (paramID == REGISTER_DEVICE)
    {
        uint8_t buf[REGISTER_ADDRESS_LEN];

        // Interface and device have changed; update register record
        Register_getParameter(REGISTER_DEVICE, buf);
        regInfo.registerAddress = 0x7E;
        regInfo.interfaceID = buf[0];
        regInfo.deviceAddress = buf[1];

        if (regInfo.interfaceID == REGISTER_INTERFACE_MCU)
        {
            // MCU, use 4 byte access
            regInfo.dataLength = sizeof(uint32_t);
        }
        else
        {
            // I2C sensors, most use two byte access
            regInfo.dataLength = sizeof(uint16_t);
        }

        // Set register address to a safe default value
        buf[0] = regInfo.dataLength;
        memcpy(&buf[1], &regInfo.registerAddress, REGISTER_ADDRESS_LEN-1);
        Register_setParameter(REGISTER_ADDRESS, REGISTER_ADDRESS_LEN, buf);
    }
}

/*********************************************************************
 * @fn      SensorTagRegister_update
 *
 * @brief   Read and store the register values
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagRegister_update(void)
{
    uint8_t data[REGISTER_DATA_LEN];

    if (readRegister(data, true))
    {
        // Set the data only if the value has changed
        Register_setParameter(REGISTER_DATA, regInfo.dataLength, data);

        regInfo.readCount++;
    }
}

/*********************************************************************
 * @fn      SensorTagRegister_reset
 *
 * @brief   Reset characteristics
 *
 * @param   none
 *
 * @return  none
 */
void SensorTagRegister_reset(void)
{
    uint8_t buf[REGISTER_ADDRESS_LEN];

    // Initialize register to point to MCU address space,
    // address of 'regInfo' structure
    regInfo.interfaceID = REGISTER_INTERFACE_MCU;
    regInfo.deviceAddress = 0; // Does not apply to MCU
    regInfo.registerAddress = (uint32_t)&regInfo;
    regInfo.dataLength = sizeof(uint32_t);
    regInfo.readCount = 0;
    memset(regInfo.data, 0, REGISTER_DATA_LEN);

    // Initialize the register service with safe data
    Register_setParameter(REGISTER_DATA, REGISTER_DATA_LEN, regInfo.data);
    buf[0] = regInfo.dataLength;
    memcpy(&buf[1], &regInfo.registerAddress, REGISTER_ADDRESS_LEN-1);
    Register_setParameter(REGISTER_ADDRESS, REGISTER_ADDRESS_LEN, buf);
    buf[0] = regInfo.interfaceID;
    buf[1] = regInfo.deviceAddress;
    Register_setParameter(REGISTER_DEVICE, REGISTER_DEVICE_LEN, buf);

    // Update the register value first time
    SensorTagRegister_update();
}


/*********************************************************************
* Private functions
*/

/*********************************************************************
 * @fn      registerChangeCB
 *
 * @brief   Callback from register service indicating a value change
 *
 * @param   paramID - identifies the characteristic that was changed
 *
 * @return  none
 */
static void registerChangeCB(uint8_t paramID)
{
  // Wake up the application thread
  SensorTag_charValueChangeCB(SERVICE_ID_REGISTER, paramID);
}

/*********************************************************************
 * @fn      readRegister
 *
 * @brief   Read registers and store the data in 'registerData'
 *
 * param    pData - buffer to contain the data read from the sensor or MCU
 *
 * param    saveData - true if data is to be saved
 *                     (for comparison to read)
 *
 * @return  true if the data have changed since the last read
 */
static bool readRegister(uint8_t *pData, bool saveData)
{
    bool dataChanged;
    RegisterInfo_t *p =  &regInfo;
    static uint8_t prevRegisterData[REGISTER_DATA_LEN] = {0x55, 0xAA};

    switch (p->interfaceID)
    {
#ifdef Board_I2C0
    // I2C interfaces
    case REGISTER_INTERFACE_I2C0:
    case REGISTER_INTERFACE_I2C1:
        {
            bool ok = true;
#ifdef Board_MPU9250_ADDR
            // Do not access MPU9250 if it is powered off
            if (p->interfaceID == REGISTER_INTERFACE_I2C1)
            {
                ok = SensorMpu9250_powerIsOn();
            }
#endif
            // Read register of an I2C sensor
            if (ok)
            {
                ok = SensorI2C_select(p->interfaceID, p->deviceAddress);
                if (ok)
                {
                    ok = SensorI2C_readReg(p->registerAddress,
                                       pData, p->dataLength);
                    SensorI2C_deselect();
                }
            }

            // Fill with 0xFF in case of failure
            if (!ok)
            {
                uint8_t i;

                for (i=0; i<p->dataLength; i++)
                {
                    pData[i]= 0xFF;
                }
            }
        }
        break;
#endif
    case REGISTER_INTERFACE_MCU:
        // Copy directly from MCU memory space
        memcpy(pData, (uint32_t*)p->registerAddress, p->dataLength);
        break;

    default:
        break;
    }

    // Check if register data has changed
    dataChanged = memcmp(pData, prevRegisterData, p->dataLength) != 0;
    if (dataChanged && saveData)
    {
        memcpy(prevRegisterData, pData, p->dataLength);
    }

    return dataChanged;
}

/*********************************************************************
 * @fn      sensor_writeRegister
 *
 * @brief   Write registers from the data stored in 'registerData'
 *
 * param    pData - buffer to contain data to be written to sensor or MCU
 *
 * @return  none
 */
static void writeRegister(uint8_t *pData)
{
    RegisterInfo_t *p = &regInfo;

    switch (p->interfaceID)
    {
#ifdef Board_I2C0
    // I2C interfaces
    case REGISTER_INTERFACE_I2C0:
    case REGISTER_INTERFACE_I2C1:
        {
            bool ok = true;
#ifdef Board_MPU9250_ADDR
            // Do not access MPU9250 if it is powered off
            if (p->interfaceID == REGISTER_INTERFACE_I2C1)
            {
                ok = SensorMpu9250_powerIsOn();
            }
#endif
            // Write register in a I2C sensor
            if (ok)
            {
                ok = SensorI2C_select(p->interfaceID, p->deviceAddress);
                if (ok)
                {
                    ok = SensorI2C_writeReg(p->registerAddress, pData, p->dataLength);
                    SensorI2C_deselect();
                }
            }
        }
        break;
#endif
    case REGISTER_INTERFACE_MCU:
        // Copy directly to memory space (dangerous!)
        memcpy((uint8_t*)p->registerAddress, pData, p->dataLength);
        break;

    default:
        break;
    }
}
#endif // EXCLUDE_REG
/*********************************************************************
*********************************************************************/

