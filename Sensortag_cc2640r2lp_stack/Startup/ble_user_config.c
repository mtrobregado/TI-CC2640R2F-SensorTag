/******************************************************************************

 @file  ble_user_config.c

 @brief This file contains user configurable variables for the BLE
        Controller and Host.

 Group: WCS, BTS
 Target Device: CC2640R2

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
 Release Name: ti-ble-3.0-stack-sdk_3_00_00
 Release Date: 2016-12-21 12:44:47
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

#include "hal_types.h"
#include "ll_common.h"
#include "osal.h"
#include "ll_user_config.h"
#include "ble_user_config.h"

#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )

#include "ble_dispatch.h"
#include "l2cap.h"

#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )

/*******************************************************************************
 * GLOBAL VARIABLES
 */
#ifdef ICALL_JT
uint32_t * icallServiceTblPtr = NULL;
#endif /* ICALL_JT */
/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn      setBleUserConfig
 *
 * @brief   Set the user configurable variables for the BLE
 *          Controller and Host.
 *
 *          Note: This function should be called at the start
 *                of stack_main.
 *
 * @param   userCfg - pointer to user configuration
 *
 * @return  none
 */
#ifdef ICALL_JT
void setBleUserConfig( icall_userCfg_t *userCfg )
{
  stackSpecific_t *stackConfig = (stackSpecific_t*) userCfg->stackConfig;
  if ( userCfg != NULL )
  {
#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    l2capUserCfg_t l2capUserCfg;

    // user reconfiguration of Host variables
    l2capUserCfg.maxNumPSM = stackConfig->maxNumPSM;
    l2capUserCfg.maxNumCoChannels = stackConfig->maxNumCoChannels;

    L2CAP_SetUserConfig( &l2capUserCfg );

#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )
    if ( stackConfig->pfnBMAlloc != NULL )
    {
      *stackConfig->pfnBMAlloc = bleDispatch_BMAlloc;
    }

    if ( stackConfig->pfnBMFree != NULL )
    {
      *stackConfig->pfnBMFree = bleDispatch_BMFree;
    }

    // user reconfiguration of Controller variables
    llUserConfig.maxNumConns  = stackConfig->maxNumConns;
    llUserConfig.numTxEntries = stackConfig->maxNumPDUs;
    llUserConfig.maxPduSize   = stackConfig->maxPduSize;

    // RF Front End Mode and Bias (based on package)
    llUserConfig.rfFeModeBias = userCfg->boardConfig->rfFeModeBias;

    // RF Override Registers
    llUserConfig.rfRegPtr     = userCfg->boardConfig->rfRegTbl;

    // Tx Power Table
    llUserConfig.txPwrTblPtr  = userCfg->boardConfig->txPwrTbl;

#ifndef DISABLE_RF_DRIVER
    // RF Driver Table
    llUserConfig.rfDrvTblPtr  = userCfg->drvTblPtr->rfDrvTbl;
#endif // !DISABLE_RF_DRIVER

#if defined(USE_CRYPTO_DRIVER) || defined(CC26XX_R2)
  if ( userCfg->drvTblPtr->cryptoDrvTbl == NULL )
  {
    LL_ASSERT( FALSE );
  }
  else
  {
    // Crypto Driver Table
    llUserConfig.cryptoDrvTblPtr = userCfg->drvTblPtr->cryptoDrvTbl;
  }
#endif // USE_CRYPTO_DRIVER | CC26XX_R2

  if ( userCfg->drvTblPtr->trngDrvTbl == NULL )
  {
    LL_ASSERT( FALSE );
  }
  else
  {
    // TRNG Driver Table
    llUserConfig.trngDrvTblPtr = userCfg->drvTblPtr->trngDrvTbl;
  }

#ifdef OSAL_SNV_EXTFLASH
  if ( userCfg->drvTblPtr->extflashDrvTbl == NULL )
  {
    LL_ASSERT( FALSE );
  }
  else
  {
    // TRNG Driver Table
    extflashDrvTblPtr = userCfg->drvTblPtr->extflashDrvTbl;
  }
#endif // OSAL_SNV_EXTFLASH

#if defined(BLE_V42_FEATURES) && (BLE_V42_FEATURES & SECURE_CONNS_CFG)
    // ECC Driver Table
    if ( userCfg->drvTblPtr->eccDrvTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      llUserConfig.eccDrvTblPtr = userCfg->drvTblPtr->eccDrvTbl;
    }
#endif //BLE_V42_FEATURES & SECURE_CONNS_CFG

    // PM Startup Margin
    llUserConfig.startupMarginUsecs = stackConfig->startupMarginUsecs;

    // save off the application's assert handler
    halAssertInit( **userCfg->appServiceInfo->assertCback, HAL_ASSERT_LEGACY_MODE_ENABLED );

    if ( userCfg->appServiceInfo->icallServiceTbl == NULL )
    {
      LL_ASSERT( FALSE );
    }
    else
    {
      icallServiceTblPtr = (uint32_t *) userCfg->appServiceInfo->icallServiceTbl;
    }

    osal_timer_init( userCfg->appServiceInfo->timerTickPeriod , userCfg->appServiceInfo->timerMaxMillisecond );
  }
  else
  {
      LL_ASSERT( FALSE );
  }

  return;
}
#else /* !(ICALL_JT) */
void setBleUserConfig( bleUserCfg_t *userCfg )
{
  if ( userCfg != NULL )
  {
#if defined( HOST_CONFIG ) && ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
    l2capUserCfg_t l2capUserCfg;

    // user reconfiguration of Host variables
    l2capUserCfg.maxNumPSM = userCfg->maxNumPSM;
    l2capUserCfg.maxNumCoChannels = userCfg->maxNumCoChannels;

    L2CAP_SetUserConfig( &l2capUserCfg );

    if ( userCfg->pfnBMAlloc != NULL )
    {
      *userCfg->pfnBMAlloc = bleDispatch_BMAlloc;
    }

    if ( userCfg->pfnBMFree != NULL )
    {
      *userCfg->pfnBMFree = bleDispatch_BMFree;
    }
#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )

    // user reconfiguration of Controller variables
    llUserConfig.maxNumConns  = userCfg->maxNumConns;
    llUserConfig.numTxEntries = userCfg->maxNumPDUs;
    llUserConfig.maxPduSize   = userCfg->maxPduSize;

    // RF Front End Mode and Bias (based on package)
    llUserConfig.rfFeModeBias = userCfg->rfFeModeBias;

    // RF Override Registers
    llUserConfig.rfRegPtr     = userCfg->rfRegTbl;

    // Tx Power Table
    llUserConfig.txPwrTblPtr  = userCfg->txPwrTbl;

#ifndef DISABLE_RF_DRIVER
    // RF Driver Table
    llUserConfig.rfDrvTblPtr  = userCfg->rfDrvTbl;
#endif // !DISABLE_RF_DRIVER

#if defined(BLE_V42_FEATURES) && (BLE_V42_FEATURES & SECURE_CONNS_CFG)
    // ECC Driver Table
    llUserConfig.eccDrvTblPtr = userCfg->eccDrvTbl;
#endif //BLE_V42_FEATURES & SECURE_CONNS_CFG

    // Crypto Driver Table
    llUserConfig.cryptoDrvTblPtr = userCfg->cryptoDrvTbl;

    // TRNG Driver Table
    llUserConfig.trngDrvTblPtr = userCfg->trngDrvTbl;

    // PM Startup Margin
    llUserConfig.startupMarginUsecs = userCfg->startupMarginUsecs;

    // save off the application's assert handler
    halAssertInit( **userCfg->assertCback, HAL_ASSERT_LEGACY_MODE_DISABLED );
  }

  return;
}
#endif /* ICALL_JT */
/*******************************************************************************
 */
