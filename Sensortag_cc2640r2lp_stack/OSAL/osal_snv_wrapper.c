/******************************************************************************

 @file  osal_snv_wrapper.c

 @brief This module defines the OSAL simple non-volatile memory functions as a
        wrapper to On Chip One Page SNV implementation.

 Group: WCS, LPC, BTS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2009-2016, Texas Instruments Incorporated
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
 Release Date: 2016-12-21 12:44:48
 *****************************************************************************/
#if !defined( OSAL_SNV )
// Use 2 page SNV by default
#define OSAL_SNV 2
#endif

// Map 2 and 0 to 2 page SNV.  0 was arbitrarily chosen to go here.
#if OSAL_SNV == 2 || OSAL_SNV == 0
#if OSAL_SNV == 0 && !defined(NO_OSAL_SNV)
#define NO_OSAL_SNV
#endif //OSAL_SNV == 0 && !defined(NO_OSAL_SNV)
#include "osal_snv.c"
#elif OSAL_SNV == 1 // This is the 1 page SNV
#include "osal_snv.h"
#include "./../../../../services/src/nv/cc26xx/nvocop.c"

#ifndef SYSTEM_ID
#define SYSTEM_ID NVINTF_SYSID_NVDRVR
#endif

// Convert a threshold percentage to bytes.
#define THRESHOLD2BYTES(x) ((FLASH_PAGE_SIZE) - (((FLASH_PAGE_SIZE) * (x)) / 100))

/*********************************************************************
 * @fn      osal_snv_init
 *
 * @brief   Initialize NV service.
 *
 * @param   none
 *
 * @return  SUCCESS if initialization succeeds. FAILURE, otherwise.
 */
uint8 osal_snv_init( void )
{  
  return NVOCOP_initNV(NULL);
}

/*********************************************************************
 * @fn      osal_snv_read
 *
 * @brief   Read data from NV.
 *
 * @param   id   - Valid NV item Id.
 * @param   len  - Length of data to read.
 * @param   *pBuf - Data is read into this buffer.
 *
 * @return  SUCCESS if successful.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8 osal_snv_read( osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  NVINTF_itemID_t nv_id;
  
  nv_id.itemID = id;
  nv_id.subID = 0;
  nv_id.systemID = SYSTEM_ID;
  
  return NVOCOP_readItem(nv_id, 0, len, pBuf);
}

/*********************************************************************
 * @fn      osal_snv_write
 *
 * @brief   Write a data item to NV.
 *
 * @param   id   - Valid NV item Id.
 * @param   len  - Length of data to write.
 * @param   *pBuf - Data to write.
 *
 * @return  SUCCESS if successful, NV_OPER_FAILED if failed.
 */
uint8 osal_snv_write( osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  NVINTF_itemID_t nv_id;
  
  nv_id.itemID = id;
  nv_id.subID = 0;
  nv_id.systemID = SYSTEM_ID;


  return NVOCOP_writeItem(nv_id, len, pBuf);
}

/*********************************************************************
 * @fn      osal_snv_compact
 *
 * @brief   Compacts NV if its usage has reached a specific threshold.
 *
 * @param   threshold - compaction threshold.
 *
 * @return  NV_INTF_SUCCESS if successful,
 *          NV_INTF_FAILURE if failed, or
 *          NV_INTF_BADPARAM if threshold invalid.
 */
uint8 osal_snv_compact( uint8 threshold )
{
  // convert percentage to approximate byte threshold.
  if (threshold <= 100)
  {
    return NVOCOP_compactNV(THRESHOLD2BYTES(threshold));
  }
  
  return NVINTF_BADPARAM;
}

#else // bad OSAL_SNV value
#error "Valid OSAL_SNV values are 0, 1, or 2!"
#endif //OSAL_SNV
