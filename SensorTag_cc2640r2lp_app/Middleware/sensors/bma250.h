/******************************************************************************

 @file  bma250.h

 @brief This file contains the BMA250 accelerator API for CC26xx.

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

#ifndef BMA250_H
#define BMA250_H

/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"

/******************************************************************************
 * DEFINES
 */

// BMA250 addressing space
#define ACC_CHIPID                  0x00 // Always 0x03
#define ACC_X_LSB                   0x02 // ACC_X_LSB[7:6] = 2 LSb of X acceleration data
#define ACC_X_MSB                   0x03 // ACC_X_MSB[7:0] = 8 MSb of X data
#define ACC_Y_LSB                   0x04
#define ACC_Y_MSB                   0x05
#define ACC_Z_LSB                   0x06
#define ACC_Z_MSB                   0x07
#define ACC_TEMP                    0x08 // Temperature data
#define ACC_INT_STATUS              0x09
#define ACC_DATA_INT_STATUS         0x0A
#define ACC_RANGE                   0x0F // 2/4/8/16 G range
#define ACC_BW                      0x10 // Filtered bandwidth
#define ACC_PM                      0x11 // Susp[7], Low_power[6], sleep_dur[4:1]
#define ACC_CONF_FILT_SHADOW        0x13
#define ACC_SOFTRESET               0x14
#define ACC_INT_ENABLE0             0x16
#define ACC_INT_ENABLE1             0x17
#define ACC_INT_MAPPING0            0x19
#define ACC_INT_MAPPING1            0x1A
#define ACC_INT_MAPPING2            0x1B
#define ACC_INT_SOURCE              0x1E
#define ACC_INT_PIN_BEHAVIOR        0x20

// Range selection definitions
#define ACC_RANGE_2G                0x03 //  3.91 mg/LSB
#define ACC_RANGE_4G                0x05 //  7.81 mg/LSB
#define ACC_RANGE_8G                0x08 // 15.62 mg/LSB
#define ACC_RANGE_16G               0x0C // 31.25 mg/LSB

// Filtered bandwidth selection (delta_t = time between successive acc samples)
#define ACC_BW_7_81HZ               0x08 // 7.81Hz bandwidth (delta_t = 64 ms)
#define ACC_BW_15_63HZ              0x09 // delta_t = 32   ms
#define ACC_BW_31_25HZ              0x0A // delta_t = 16   ms
#define ACC_BW_62_5HZ               0x0B // delta_t =  8   ms
#define ACC_BW_125HZ                0x0C // delta_t =  4   ms
#define ACC_BW_250HZ                0x0D // delta_t =  2   ms
#define ACC_BW_500HZ                0x0E // delta_t =  1   ms
#define ACC_BW_1000HZ               0x0F // delta_t =  0.5 ms

#define ACC_PM_SUSP                 0x80 // Power mode register (0x11), bit 7
#define ACC_PM_LP                   0x40 // Low power mode
#define ACC_PM_SLEEP_10MS           0x14
#define ACC_PM_SLEEP_25MS           0x16
#define ACC_PM_SLEEP_50MS           0x18

// Interrupt enable bitmasks (for use with registers ACC_INT_ENABLEx [x=0,1] )
#define ACC_INT_FLAT_EN             0x80 // Bit in register 0x16
#define ACC_INT_ORIENT_EN           0x40 //          "
#define ACC_INT_S_TAP_EN            0x20 //          "
#define ACC_INT_D_TAP_EN            0x10 //          "
#define ACC_INT_SLOPE_Z_EN          0x04 //          "
#define ACC_INT_SLOPE_Y_EN          0x02 //          "
#define ACC_INT_SLOPE_X_EN          0x01 //          "
#define ACC_INT_DATA_EN             0x10 // Bit in register 0x17
#define ACC_INT_LOW_EN              0x08 //          "
#define ACC_INT_HIGH_Z_EN           0x04 //          "
#define ACC_INT_HIGH_Y_EN           0x02 //          "
#define ACC_INT_HIGH_X_EN           0x01 //          "

// Interrupt mapping bitmasks (for use with registers ACC_INT_MAPPINGx [x=0,1,2] )
#define ACC_INT_MAP_FLAT            0x80 // For pin INT1 (INT2), bit in register 0x19 (0x1B)
#define ACC_INT_MAP_ORIENT          0x40 //                   "
#define ACC_INT_MAP_S_TAP           0x20 //                   "
#define ACC_INT_MAP_D_TAP           0x10 //                   "
#define ACC_INT_MAP_SLOPE           0x04 //                   "
#define ACC_INT_MAP_HIGH            0x02 //                   "
#define ACC_INT_MAP_LOW             0x01 //                   "
#define ACC_INT1_MAP_DATA           0x01 // New data IRQ to pin INT1, bit in register 0x1A
#define ACC_INT2_MAP_DATA           0x80 // New data IRQ to pin INT2, bit in register 0x1A

// Interrupt source bitmasks (for use with register ACC_INT_SOURCE)
#define ACC_INT_SRC_DATA_FILT       0x20
#define ACC_INT_SRC_TAP_FILT        0x01
#define ACC_INT_SRC_SLOPE_FILT      0x04
#define ACC_INT_SRC_HIGH_FILT       0x02
#define ACC_INT_SRC_LOW_FILT        0x01

// Interrupt pin behavior bitmasks (for use with register (Open drive/push-pull and active level 0/1)
#define ACC_INT2_OD                 0x08
#define ACC_INT2_LVL                0x04
#define ACC_INT1_OD                 0x02
#define ACC_INT1_LVL                0x01

// Perform soft reset
#define ACC_SOFTRESET_EN            0xB6 // Soft reset by writing 0xB6 to softreset register


/******************************************************************************
 * MACROS
 */
// Wait 1 [ms]
#define WAIT_1MS()      {for(unsigned short i=0;i<32000;i++)asm("NOP"); }

// Wait t [ms]
#define WAIT_MS(t)                      \
    do{                                 \
        for(uint8_t i = 0; i<t; i++)      \
            WAIT_1MS();                 \
    }while(0)


/******************************************************************************
 * FUNCTION PROTOTYPES
 */
void Acc_init(void);
void Acc_stop(void);
void Acc_writeReg(uint8_t reg, uint8_t val);
void Acc_readReg(uint8_t reg, uint8_t *pVal);
void Acc_readAcc(int8_t *pXVal, int8_t *pYVal, int8_t *pZVal);
void Acc_readAcc16(int16_t *pXVal, int16_t *pYVal, int16_t *pZVal);


#endif // BMA250_H
