/******************************************************************************

 @file  ble_dispatch_JT.c

 @brief ICall BLE Stack Dispatcher for embedded and NP/Serial messages.

 Group: WCS, BTS
 Target Device: CC2640R2

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
 Release Name: ti-ble-3.0-stack-sdk_3_00_00
 Release Date: 2016-12-21 12:44:47
 *****************************************************************************/


#ifdef ICALL_LITE

/*********************************************************************
 * INCLUDES
 */
#include "osal_snv.h"
#include "osal_bufmgr.h"

#include "hal_trng_wrapper.h"

#include "sm.h"
#include "gap.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "hci_tl.h"
#include "linkdb.h"
#include "npi.h"

#include "gattservapp.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"

#if defined ( GATT_TEST ) || defined ( GATT_QUAL )
  #include "gatttest.h"
#endif

#include "icall_apimsg.h"
#include "hci_ext.h"
#include "ble_dispatch.h"
#include "ble_dispatch_lite.h"

#include "ll_common.h"

#include <icall_lite_translation.h>

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
#ifndef STACK_LIBRARY
const uint32_t bleAPItable[] =
{
  (uint32)HCI_ReadRssiCmd,                                   // JT_INDEX[0]
  (uint32)HCI_SetEventMaskCmd,                               // JT_INDEX[1]
  (uint32)HCI_SetEventMaskPage2Cmd,                          // JT_INDEX[2]
  (uint32)HCI_ResetCmd,                                      // JT_INDEX[3]
  (uint32)HCI_ReadLocalVersionInfoCmd,                       // JT_INDEX[4]
  (uint32)HCI_ReadLocalSupportedCommandsCmd,                 // JT_INDEX[5]
  (uint32)HCI_ReadLocalSupportedFeaturesCmd,                 // JT_INDEX[6]
  (uint32)HCI_ReadBDADDRCmd,                                 // JT_INDEX[7]
  (uint32)HCI_LE_ReceiverTestCmd,                            // JT_INDEX[8]
  (uint32)HCI_LE_TransmitterTestCmd,                         // JT_INDEX[9]
  (uint32)HCI_LE_TestEndCmd,                                 // JT_INDEX[10]
  (uint32)HCI_LE_EncryptCmd,                                 // JT_INDEX[11]
  (uint32)HCI_LE_RandCmd,                                    // JT_INDEX[12]
  (uint32)HCI_LE_ReadSupportedStatesCmd,                     // JT_INDEX[13]
  (uint32)HCI_LE_ReadWhiteListSizeCmd,                       // JT_INDEX[14]
  (uint32)HCI_LE_ClearWhiteListCmd,                          // JT_INDEX[15]
  (uint32)HCI_LE_AddWhiteListCmd,                            // JT_INDEX[16]
  (uint32)HCI_LE_RemoveWhiteListCmd,                         // JT_INDEX[17]
  (uint32)HCI_LE_SetEventMaskCmd,                            // JT_INDEX[18]
  (uint32)HCI_LE_ReadLocalSupportedFeaturesCmd,              // JT_INDEX[19]
  (uint32)HCI_LE_ReadBufSizeCmd,                             // JT_INDEX[20]
  (uint32)HCI_LE_SetRandomAddressCmd,                        // JT_INDEX[21]
#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
  (uint32)HCI_LE_ReadAdvChanTxPowerCmd,                      // JT_INDEX[22]
#else // !(defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG)))
  (uint32)icall_liteErrorFunction,
#endif //  (defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG)))
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_DisconnectCmd,                                 // JT_INDEX[23]
  (uint32)HCI_LE_ReadChannelMapCmd,                          // JT_INDEX[24]
  (uint32)HCI_LE_ReadRemoteUsedFeaturesCmd,                  // JT_INDEX[25]
  (uint32)HCI_ReadRemoteVersionInfoCmd,                      // JT_INDEX[26]
  (uint32)HCI_ReadTransmitPowerLevelCmd,                     // JT_INDEX[27]
  (uint32)HCI_SetControllerToHostFlowCtrlCmd,                // JT_INDEX[28]
  (uint32)HCI_HostBufferSizeCmd,                             // JT_INDEX[29]
  (uint32)HCI_HostNumCompletedPktCmd,                        // JT_INDEX[30]
#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG))
  (uint32)HCI_LE_SetHostChanClassificationCmd,               // JT_INDEX[31]
#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG)))
  (uint32)icall_liteErrorFunction,
#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG)))
#if defined(CTRL_V42_CONFIG) && (CTRL_V42_CONFIG & EXT_DATA_LEN_CFG) &&            \
defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_LE_ReadMaxDataLenCmd,                          // JT_INDEX[32]
  (uint32)HCI_LE_SetDataLenCmd,                              // JT_INDEX[33]
  (uint32)HCI_LE_WriteSuggestedDefaultDataLenCmd,            // JT_INDEX[34]
  (uint32)HCI_LE_ReadSuggestedDefaultDataLenCmd,             // JT_INDEX[35]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif //  (...)
#if defined(CTRL_V41_CONFIG) && (CTRL_V41_CONFIG & CONN_PARAM_REQ_CFG) &&      \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_ReadAuthPayloadTimeoutCmd,                     // JT_INDEX[36]
  (uint32)HCI_WriteAuthPayloadTimeoutCmd,                    // JT_INDEX[37]
  (uint32)HCI_LE_RemoteConnParamReqReplyCmd,                 // JT_INDEX[38]
  (uint32)HCI_LE_RemoteConnParamReqNegReplyCmd,              // JT_INDEX[39]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_V42_CONFIG) && (CTRL_V42_CONFIG & PRIVACY_1_2_CFG)
  (uint32)HCI_LE_AddDeviceToResolvingListCmd,                // JT_INDEX[40]
  (uint32)HCI_LE_RemoveDeviceFromResolvingListCmd,           // JT_INDEX[41]
  (uint32)HCI_LE_ClearResolvingListCmd,                      // JT_INDEX[42]
  (uint32)HCI_LE_ReadResolvingListSizeCmd,                   // JT_INDEX[43]
  (uint32)HCI_LE_ReadPeerResolvableAddressCmd,               // JT_INDEX[44]
  (uint32)HCI_LE_ReadLocalResolvableAddressCmd,              // JT_INDEX[45]
  (uint32)HCI_LE_SetAddressResolutionEnableCmd,              // JT_INDEX[46]
  (uint32)HCI_LE_SetResolvablePrivateAddressTimeoutCmd,      // JT_INDEX[47]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_V42_CONFIG) && (CTRL_V42_CONFIG & SECURE_CONNS_CFG)
  (uint32)HCI_LE_ReadLocalP256PublicKeyCmd,                  // JT_INDEX[48]
  (uint32)HCI_LE_GenerateDHKeyCmd,                           // JT_INDEX[49]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) &&  \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_LE_ReadPhyCmd,                                 // JT_INDEX[50]
  (uint32)HCI_LE_SetDefaultPhyCmd,                           // JT_INDEX[51]
  (uint32)HCI_LE_SetPhyCmd,                                  // JT_INDEX[52]
  (uint32)HCI_LE_EnhancedRxTestCmd,                          // JT_INDEX[53]
  (uint32)HCI_LE_EnhancedTxTestCmd,                          // JT_INDEX[54]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  (uint32)HCI_LE_SetAdvParamCmd,                             // JT_INDEX[55]
  (uint32)HCI_LE_SetAdvDataCmd,                              // JT_INDEX[56]
  (uint32)HCI_LE_SetScanRspDataCmd,                          // JT_INDEX[57]
  (uint32)HCI_LE_SetAdvEnableCmd,                            // JT_INDEX[58]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
  (uint32)HCI_LE_SetScanParamCmd,                            // JT_INDEX[59]
  (uint32)HCI_LE_SetScanEnableCmd,                           // JT_INDEX[60]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
  (uint32)HCI_LE_CreateConnCmd,                              // JT_INDEX[61]
  (uint32)HCI_LE_CreateConnCancelCmd,                        // JT_INDEX[62]
  (uint32)HCI_LE_StartEncyptCmd,                             // JT_INDEX[63]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_LE_ConnUpdateCmd,                              // JT_INDEX[64]
#else // !(...)
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  (uint32)HCI_LE_LtkReqReplyCmd,                             // JT_INDEX[65]
  (uint32)HCI_LE_LtkReqNegReplyCmd,                          // JT_INDEX[66]
#else // !(...)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (...)
  (uint32)HCI_EXT_SetTxPowerCmd,                             // JT_INDEX[67]
  (uint32)HCI_EXT_BuildRevisionCmd,                          // JT_INDEX[68]
  (uint32)HCI_EXT_DelaySleepCmd,                             // JT_INDEX[69]
  (uint32)HCI_EXT_DecryptCmd,                                // JT_INDEX[70]
  (uint32)HCI_EXT_EnablePTMCmd,                              // JT_INDEX[71]
  (uint32)HCI_EXT_ModemTestTxCmd,                            // JT_INDEX[72]
  (uint32)HCI_EXT_ModemHopTestTxCmd,                         // JT_INDEX[73]
  (uint32)HCI_EXT_ModemTestRxCmd,                            // JT_INDEX[74]
  (uint32)HCI_EXT_EndModemTestCmd,                           // JT_INDEX[75]
  (uint32)HCI_EXT_SetBDADDRCmd,                              // JT_INDEX[76]
  (uint32)HCI_EXT_ResetSystemCmd,                            // JT_INDEX[77]
  (uint32)HCI_EXT_SetLocalSupportedFeaturesCmd,              // JT_INDEX[78]
  (uint32)HCI_EXT_SetMaxDtmTxPowerCmd,                       // JT_INDEX[79]
  (uint32)HCI_EXT_SetRxGainCmd,                              // JT_INDEX[80]
  (uint32)HCI_EXT_ExtendRfRangeCmd,                          // JT_INDEX[81]
  (uint32)HCI_EXT_HaltDuringRfCmd,                           // JT_INDEX[82]
  (uint32)HCI_EXT_ClkDivOnHaltCmd,                           // JT_INDEX[83]
  (uint32)HCI_EXT_DeclareNvUsageCmd,                         // JT_INDEX[84]
  (uint32)HCI_EXT_MapPmIoPortCmd,                            // JT_INDEX[85]
  (uint32)HCI_EXT_SetFreqTuneCmd,                            // JT_INDEX[86]
  (uint32)HCI_EXT_SaveFreqTuneCmd,                           // JT_INDEX[87]
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_EXT_ConnEventNoticeCmd,                        // JT_INDEX[88]
  (uint32)HCI_EXT_DisconnectImmedCmd,                        // JT_INDEX[89]
  (uint32)HCI_EXT_PacketErrorRateCmd,                        // JT_INDEX[90]
  (uint32)HCI_EXT_NumComplPktsLimitCmd,                      // JT_INDEX[91]
  (uint32)HCI_EXT_OnePktPerEvtCmd,                           // JT_INDEX[92]
  (uint32)HCI_EXT_SetSCACmd,                                 // JT_INDEX[93]
  (uint32)HCI_EXT_GetConnInfoCmd,                            // JT_INDEX[94]
  (uint32)HCI_EXT_OverlappedProcessingCmd,                   // JT_INDEX[95]
#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))
#if defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))
  (uint32)HCI_EXT_AdvEventNoticeCmd,                         // JT_INDEX[96]
#else // !(defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG)))
  (uint32)icall_liteErrorFunction,
#endif //  (defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG))Y)
#if ( defined(BLE_VS_FEATURES) && (BLE_VS_FEATURES & SCAN_EVT_NOTICE_CFG) && ( defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)) )
  (uint32)HCI_EXT_ScanEventNoticeCmd,                        // JT_INDEX[97]
#else // !(defined(BLE_VS_FEATURES) && (BLE_VS_FEATURES & SCAN_EVT_NOTICE_CFG) && ( defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)))
  (uint32)icall_liteErrorFunction,
#endif //  (defined(BLE_VS_FEATURES) && (BLE_VS_FEATURES & SCAN_EVT_NOTICE_CFG) && ( defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)))
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_EXT_SetFastTxResponseTimeCmd,                  // JT_INDEX[98]
  (uint32)HCI_EXT_SetSlaveLatencyOverrideCmd,                // JT_INDEX[99]
#else // !(defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG)))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif //  (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG)))
#if defined(BLE_VS_FEATURES) && (BLE_VS_FEATURES & SCAN_REQ_RPT_CFG) &&        \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  (uint32)HCI_EXT_ScanReqRptCmd,                             // JT_INDEX[100]
#else // !(...)
  (uint32)icall_liteErrorFunction,
#endif // (...)
#if defined(CTRL_V42_CONFIG) && (CTRL_V42_CONFIG & EXT_DATA_LEN_CFG) &&        \
    defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  (uint32)HCI_EXT_SetMaxDataLenCmd,                          // JT_INDEX[101]
#else // !(...)
  (uint32)icall_liteErrorFunction,
#endif // (...)
#ifdef LL_TEST_MODE
  (uint32)HCI_EXT_LLTestModeCmd,                             // JT_INDEX[102]
#else // !(...)
  (uint32)icall_liteErrorFunction,
#endif // (...)
  (uint32)HCI_SendDataPkt,                                   // JT_INDEX[103]
  (uint32)HCI_CommandCompleteEvent,                          // JT_INDEX[104]
#if defined(HCI_TL_FULL)
  (uint32)HCI_TL_getCmdResponderID,                          // JT_INDEX[105]
#else // !defined(HCI_TL_FULL)
  (uint32)icall_liteErrorFunction,
#endif // defined(HCI_TL_FULL)
#if defined(HOST_CONFIG)
  (uint32)GGS_SetParameter,                                  // JT_INDEX[106]
  (uint32)GGS_AddService,                                    // JT_INDEX[107]
  (uint32)GGS_GetParameter,                                  // JT_INDEX[108]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if ( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )
  (uint32)GGS_RegisterAppCBs,                                // JT_INDEX[109]
#else // !( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & PERIPHERAL_CFG ) & defined ( GAP_PRIVACY_RECONNECT )
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))
  (uint32)GGS_SetParamValue,                                 // JT_INDEX[110]
  (uint32)GGS_GetParamValue,                                 // JT_INDEX[111]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
#if defined(GAP_BOND_MGR)
  (uint32)GAPBondMgr_SetParameter,                           // JT_INDEX[112]
  (uint32)GAPBondMgr_GetParameter,                           // JT_INDEX[113]
  (uint32)GAPBondMgr_LinkEst,                                // JT_INDEX[114]
  (uint32)GAPBondMgr_LinkTerm,                               // JT_INDEX[115]
  (uint32)GAPBondMgr_Register,                               // JT_INDEX[116]
  (uint32)GAPBondMgr_PasscodeRsp,                            // JT_INDEX[117]
  (uint32)GAPBondMgr_ResolveAddr,                            // JT_INDEX[118]
#else // !( GAP_BOND_MGR )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( GAP_BOND_MGR )
#if defined(GAP_BOND_MGR) && !defined(GATT_NO_SERVICE_CHANGED)
  (uint32)GAPBondMgr_ServiceChangeInd,                       // JT_INDEX[119]
  (uint32)GAPBondMgr_SlaveReqSecurity,                       // JT_INDEX[120]
#else // !( defined(GAP_BOND_MGR) && !defined(GATT_NO_SERVICE_CHANGED) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( defined(GAP_BOND_MGR) && !defined(GATT_NO_SERVICE_CHANGED) )
#if (  !defined(GATT_DB_OFF_CHIP) && defined(GAP_BOND_MGR) )
  (uint32)GAPBondMgr_UpdateCharCfg,                          // JT_INDEX[121]
  (uint32)GAPBondMgr_ProcessGAPMsg,                          // JT_INDEX[122]
#else // !( !defined(GATT_DB_OFF_CHIP) && defined(GAP_BOND_MGR)  )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( !defined(GATT_DB_OFF_CHIP) && defined(GAP_BOND_MGR) )
#if defined(HOST_CONFIG)
  (uint32)GAP_DeviceInit,                                    // JT_INDEX[123]
  (uint32)GAP_RegisterForMsgs,                               // JT_INDEX[124]
  (uint32)GAP_SetParamValue,                                 // JT_INDEX[125]
  (uint32)GAP_GetParamValue,                                 // JT_INDEX[126]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if (HOST_CONFIG & (PERIPHERAL_CFG | BROADCASTER_CFG))
  (uint32)GAP_SetAdvToken,                                   // JT_INDEX[127]
  (uint32)GAP_RemoveAdvToken,                                // JT_INDEX[128]
  (uint32)GAP_UpdateAdvTokens,                               // JT_INDEX[129]
  (uint32)GAP_EndDiscoverable,                               // JT_INDEX[130]
  (uint32)GAP_UpdateAdvertisingData,                         // JT_INDEX[131]
  (uint32)GAP_MakeDiscoverable,                              // JT_INDEX[132]
#else // !( (HOST_CONFIG & (PERIPHERAL_CFG | BROADCASTER_CFG)) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( (HOST_CONFIG & (PERIPHERAL_CFG | BROADCASTER_CFG)) )
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))
  (uint32)GAP_UpdateLinkParamReq,                            // JT_INDEX[133]
  (uint32)GAP_TerminateLinkReq,                              // JT_INDEX[134]
  (uint32)GAP_UpdateLinkParamReqReply,                       // JT_INDEX[135]
  (uint32)GAP_ConfigDeviceAddr,                              // JT_INDEX[136]
#else // !( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
#if ( HOST_CONFIG & ( CENTRAL_CFG ))
  (uint32)GAP_EstablishLinkReq,                              // JT_INDEX[137]
#else // !( HOST_CONFIG & ( CENTRAL_CFG ) )
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & ( CENTRAL_CFG ) )
#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ))
  (uint32)GAP_DeviceDiscoveryCancel,                         // JT_INDEX[138]
  (uint32)GAP_DeviceDiscoveryRequest,                        // JT_INDEX[139]
#else // !( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
#if defined(HOST_CONFIG) && !defined(NO_BLE_SECURITY)
  (uint32)GAP_TerminateAuth,                                 // JT_INDEX[140]
  (uint32)GAP_Authenticate,                                  // JT_INDEX[141]
  (uint32)GAP_PasskeyUpdate,                                 // JT_INDEX[142]
  (uint32)GAP_Signable,                                      // JT_INDEX[143]
  (uint32)GAP_Bond,                                          // JT_INDEX[144]
#else // !(defined(HOST_CONFIG) && !defined(NO_BLE_SECURITY))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif //  (defined(HOST_CONFIG) && !defined(NO_BLE_SECURITY))
#if !defined (BLE_V42_FEATURES) || !(BLE_V42_FEATURES & PRIVACY_1_2_CFG)
  (uint32)GAP_ResolvePrivateAddr,                            // JT_INDEX[145]
#else // !(BLE_V42_FEATURES) || !(BLE_V42_FEATURES & PRIVACY_1_2_CFG)
  (uint32)icall_liteErrorFunction,
#endif //  (BLE_V42_FEATURES) || !(BLE_V42_FEATURES & PRIVACY_1_2_CFG)
#if (HOST_CONFIG & PERIPHERAL_CFG) && !defined(NO_BLE_SECURITY)
  (uint32)GAP_SendSlaveSecurityRequest,                      // JT_INDEX[146]
#else // !(HOST_CONFIG & PERIPHERAL_CFG) && !defined(NO_BLE_SECURITY)
  (uint32)icall_liteErrorFunction,
#endif //  (HOST_CONFIG & PERIPHERAL_CFG) && !defined(NO_BLE_SECURITY)
#if defined(HOST_CONFIG)
  (uint32)L2CAP_ConnParamUpdateReq,                          // JT_INDEX[147]
  (uint32)L2CAP_ParseParamUpdateReq,                         // JT_INDEX[148]
  (uint32)L2CAP_ParseInfoReq,                                // JT_INDEX[149]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
  (uint32)L2CAP_RegisterPsm,                                 // JT_INDEX[150]
  (uint32)L2CAP_DeregisterPsm,                               // JT_INDEX[151]
  (uint32)L2CAP_PsmInfo,                                     // JT_INDEX[152]
  (uint32)L2CAP_PsmChannels,                                 // JT_INDEX[153]
  (uint32)L2CAP_ChannelInfo,                                 // JT_INDEX[154]
  (uint32)L2CAP_SendSDU,                                     // JT_INDEX[155]
  (uint32)L2CAP_ConnectReq,                                  // JT_INDEX[156]
  (uint32)L2CAP_ConnectRsp,                                  // JT_INDEX[157]
  (uint32)L2CAP_DisconnectReq,                               // JT_INDEX[158]
  (uint32)L2CAP_FlowCtrlCredit,                              // JT_INDEX[159]
#else // !( (BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( (BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG) )
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))
  (uint32)L2CAP_RegisterFlowCtrlTask,                        // JT_INDEX[160]
#else // !( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))
  (uint32)L2CAP_SetParamValue,                               // JT_INDEX[161]
  (uint32)L2CAP_GetParamValue,                               // JT_INDEX[162]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))
  (uint32)L2CAP_InfoReq,                                     // JT_INDEX[163]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))
#if defined(HOST_CONFIG)
  (uint32)GATT_Indication,                                   // JT_INDEX[164]
  (uint32)GATT_Notification,                                 // JT_INDEX[165]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))
  (uint32)GATT_RegisterForMsgs,                              // JT_INDEX[166]
  (uint32)GATT_SendRsp,                                      // JT_INDEX[167]
  (uint32)GATT_SetHostToAppFlowCtrl,                         // JT_INDEX[168]
  (uint32)GATT_AppCompletedMsg,                              // JT_INDEX[169]
#else // !( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
#if ( (HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )) && (!defined(GATT_NO_CLIENT)))
  (uint32)GATT_InitClient,                                   // JT_INDEX[170]
  (uint32)GATT_RegisterForInd,                               // JT_INDEX[171]
#else // !( (HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )) && (!defined(GATT_NO_CLIENT)) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ((HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )) && (!defined(GATT_NO_CLIENT)) )
#if defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT)
  (uint32)GATT_ExchangeMTU,                                  // JT_INDEX[172]
  (uint32)GATT_DiscAllPrimaryServices,                       // JT_INDEX[173]
  (uint32)GATT_DiscPrimaryServiceByUUID,                     // JT_INDEX[174]
  (uint32)GATT_DiscAllChars,                                 // JT_INDEX[175]
  (uint32)GATT_DiscCharsByUUID,                              // JT_INDEX[176]
  (uint32)GATT_DiscAllCharDescs,                             // JT_INDEX[177]
  (uint32)GATT_ReadCharValue,                                // JT_INDEX[178]
  (uint32)GATT_ReadUsingCharUUID,                            // JT_INDEX[179]
  (uint32)GATT_ReadLongCharValue,                            // JT_INDEX[180]
  (uint32)GATT_ReadMultiCharValues,                          // JT_INDEX[181]
  (uint32)GATT_WriteNoRsp,                                   // JT_INDEX[182]
  (uint32)GATT_SignedWriteNoRsp,                             // JT_INDEX[183]
  (uint32)GATT_WriteCharValue,                               // JT_INDEX[184]
  (uint32)GATT_WriteLongCharValue,                           // JT_INDEX[185]
  (uint32)GATT_WriteLongCharDesc,                            // JT_INDEX[186]
  (uint32)GATT_ReliableWrites,                               // JT_INDEX[187]
  (uint32)GATT_ReadCharDesc,                                 // JT_INDEX[188]
  (uint32)GATT_ReadLongCharDesc,                             // JT_INDEX[189]
  (uint32)GATT_WriteCharDesc,                                // JT_INDEX[190]
#else // !(defined(HOST_CONFIG) !defined(GATT_NO_CLIENT) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (defined(HOST_CONFIG) !defined(GATT_NO_CLIENT) )
#if ( HOST_CONFIG & ( PERIPHERAL_CFG ))
  (uint32)GATT_GetNextHandle,                                // JT_INDEX[191]
#else // !( HOST_CONFIG & ( PERIPHERAL_CFG ) )
  (uint32)icall_liteErrorFunction,
#endif // ( HOST_CONFIG & ( PERIPHERAL_CFG ) )
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))
  (uint32)GATT_PrepareWriteReq,                              // JT_INDEX[192]
  (uint32)GATT_ExecuteWriteReq,                              // JT_INDEX[193]
  (uint32)GATT_FindUUIDRec,                                  // JT_INDEX[194]
  (uint32)GATT_RegisterService,                              // JT_INDEX[195]
  (uint32)GATT_DeregisterService,                            // JT_INDEX[196]
  (uint32)GATT_FindIncludedServices,                         // JT_INDEX[197]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))
  (uint32)GATT_RegisterForReq,                               // JT_INDEX[198]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))
#if defined(HOST_CONFIG)
  (uint32)GATTServApp_RegisterService,                       // JT_INDEX[199]
  (uint32)GATTServApp_AddService,                            // JT_INDEX[200]
  (uint32)GATTServApp_DeregisterService,                     // JT_INDEX[201]
  (uint32)GATTServApp_SetParameter,                          // JT_INDEX[202]
  (uint32)GATTServApp_GetParameter,                          // JT_INDEX[203]
  (uint32)GATTServApp_SendCCCUpdatedEvent,                   // JT_INDEX[204]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if !defined(GATT_NO_SERVICE_CHANGED)
  (uint32)GATTServApp_SendServiceChangedInd,                 // JT_INDEX[205]
#else // !( !defined(GATT_NO_SERVICE_CHANGED) )
  (uint32)icall_liteErrorFunction,
#endif // ( !defined(GATT_NO_SERVICE_CHANGED) )
#ifdef ATT_DELAYED_REQ
  (uint32)GATTServApp_ReadRsp,                               // JT_INDEX[206]
#else // !( ATT_DELAYED_REQ )
  (uint32)icall_liteErrorFunction,
#endif // ( ATT_DELAYED_REQ )
#ifdef GATT_QUAL
  (uint32)GATTQual_AddService,                               // JT_INDEX[207]
#else // !( GATT_QUAL )
  (uint32)icall_liteErrorFunction,
#endif // ( GATT_QUAL )
#ifdef GATT_TEST
  (uint32)GATTTest_AddService,                               // JT_INDEX[208]
#else // !( GATT_TEST )
  (uint32)icall_liteErrorFunction,
#endif // ( GATT_TEST )
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))
  (uint32)GATTServApp_GetParamValue,                         // JT_INDEX[209]
  (uint32)GATTServApp_SetParamValue,                         // JT_INDEX[210]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) ))
  (uint32)GATTServApp_RegisterForMsg,                        // JT_INDEX[211]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)  ))
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL)   ))
#if defined(HOST_CONFIG)
  (uint32)linkDB_NumActive,                                  // JT_INDEX[212]
  (uint32)linkDB_GetInfo,                                    // JT_INDEX[213]
  (uint32)linkDB_State,                                      // JT_INDEX[214]
  (uint32)linkDB_NumConns,                                   // JT_INDEX[215]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if defined(HOST_CONFIG) && defined(BLE_V42_FEATURES) && (BLE_V42_FEATURES & SECURE_CONNS_CFG)
  (uint32)SM_GetScConfirmOob,                                // JT_INDEX[216]
#else // !(defined(HOST_CONFIG) && defined(BLE_V42_FEATURES) && (BLE_V42_FEATURES & SECURE_CONNS_CFG )
  (uint32)icall_liteErrorFunction,
#endif // (defined(HOST_CONFIG) && defined(BLE_V42_FEATURES) && (BLE_V42_FEATURES & SECURE_CONNS_CFG )
#if defined(HCI_TL_FULL) && defined(BLE_V42_FEATURES) && defined(HOST_CONFIG) && (BLE_V42_FEATURES & SECURE_CONNS_CFG)
  (uint32)SM_GetEccKeys,                                     // JT_INDEX[217]
  (uint32)SM_GetDHKey,                                       // JT_INDEX[218]
  (uint32)SM_RegisterTask,                                   // JT_INDEX[219]
#else // !(#if defined(HCI_TL_FULL) && defined(BLE_V42_FEATURES) && defined(HOST_CONFIG) && (BLE_V42_FEATURES & SECURE_CONNS_CFG) )
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // (#if defined(HCI_TL_FULL) && defined(BLE_V42_FEATURES) && defined(HOST_CONFIG) && (BLE_V42_FEATURES & SECURE_CONNS_CFG))
#if defined(HOST_CONFIG)
  (uint32)ATT_ErrorRsp,                                      // JT_INDEX[220]
  (uint32)ATT_ReadBlobRsp,                                   // JT_INDEX[221]
  (uint32)ATT_ExecuteWriteRsp,                               // JT_INDEX[222]
  (uint32)ATT_WriteRsp,                                      // JT_INDEX[223]
  (uint32)ATT_ReadRsp,                                       // JT_INDEX[224]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if ( defined(HOST_CONFIG) && defined(HCI_TL_FULL))
  (uint32)ATT_ParseExchangeMTUReq,                           // JT_INDEX[225]
  (uint32)ATT_ExchangeMTURsp,                                // JT_INDEX[226]
  (uint32)ATT_FindInfoRsp,                                   // JT_INDEX[227]
  (uint32)ATT_FindByTypeValueRsp,                            // JT_INDEX[228]
  (uint32)ATT_ReadByTypeRsp,                                 // JT_INDEX[229]
  (uint32)ATT_ReadMultiRsp,                                  // JT_INDEX[230]
  (uint32)ATT_ReadByGrpTypeRsp,                              // JT_INDEX[231]
  (uint32)ATT_PrepareWriteRsp,                               // JT_INDEX[232]
#else // !defined(HOST_CONFIG)
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // defined(HOST_CONFIG)
#if defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT)
  (uint32)ATT_HandleValueCfm,                                // JT_INDEX[233]
#else // !(defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT) )
  (uint32)icall_liteErrorFunction,
#endif // (defined(HOST_CONFIG) && !defined(GATT_NO_CLIENT) )
#if ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES) )))
  (uint32)ATT_SetParamValue,                                 // JT_INDEX[234]
  (uint32)ATT_GetParamValue,                                 // JT_INDEX[235]
#else // !( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
  (uint32)icall_liteErrorFunction,
  (uint32)icall_liteErrorFunction,
#endif // ( defined(HOST_CONFIG) && ( defined(HCI_TL_FULL) && ( !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES))  ))
  (uint32)osal_snv_read,                                     // JT_INDEX[236]
  (uint32)osal_snv_write,                                    // JT_INDEX[237]
#if !defined(HCI_TL_NONE)
  (uint32)NPI_RegisterTask,                                  // JT_INDEX[238]
#else // !( !defined(HCI_TL_NONE) )
  (uint32)icall_liteErrorFunction,
#endif // ( !defined(HCI_TL_NONE) )
/* this should remain last, so that for any configuration, 
the revision needs to be read. this enable quick detection of bad alignement 
in the table */
  (uint32)buildRevision,                                     // JT_INDEX[239]
};
#endif /* STACK_LIBRARY */
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */


/*********************************************************************
 * NETWORK LAYER CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

#endif /* ICALL_LITE */

/*********************************************************************
*********************************************************************/



