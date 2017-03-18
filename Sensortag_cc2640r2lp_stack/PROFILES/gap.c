/******************************************************************************

 @file  gap.c

 @brief This file contains the GAP Configuration API.

 Group: WCS, BTS
 Target Device: CC2640R2

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
 Release Name: ti-ble-3.0-stack-sdk_3_00_00
 Release Date: 2016-12-21 12:44:47
 *****************************************************************************/

#include "bcomdef.h"
#include "gap.h"
#include "sm.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
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

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * Called to setup the device.  Call just once.
 *
 * Public function defined in gap.h.
 */
bStatus_t GAP_DeviceInit(  uint8 taskID,
                           uint8 profileRole,
                           uint8 maxScanResponses,
                           uint8 *pIRK,
                           uint8 *pSRK,
                           uint32 *pSignCounter )
{
  bStatus_t stat = INVALIDPARAMETER;   // Return status

  // Valid profile roles and supported combinations
  switch ( profileRole )
  {
    case GAP_PROFILE_BROADCASTER:
      #if ( HOST_CONFIG & ( BROADCASTER_CFG | PERIPHERAL_CFG ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case GAP_PROFILE_OBSERVER:
      #if ( HOST_CONFIG & ( OBSERVER_CFG | CENTRAL_CFG ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case GAP_PROFILE_PERIPHERAL:
      #if ( HOST_CONFIG & PERIPHERAL_CFG )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case GAP_PROFILE_CENTRAL:
      #if ( HOST_CONFIG & CENTRAL_CFG )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_BROADCASTER | GAP_PROFILE_OBSERVER):
      #if ( ( HOST_CONFIG & ( BROADCASTER_CFG | PERIPHERAL_CFG ) ) && \
            ( HOST_CONFIG & ( OBSERVER_CFG | CENTRAL_CFG ) ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_PERIPHERAL | GAP_PROFILE_OBSERVER):
      #if ( ( HOST_CONFIG & PERIPHERAL_CFG ) && \
            ( HOST_CONFIG & ( OBSERVER_CFG | CENTRAL_CFG ) ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    case (GAP_PROFILE_CENTRAL | GAP_PROFILE_BROADCASTER):
      #if ( ( HOST_CONFIG & CENTRAL_CFG ) && \
            ( HOST_CONFIG & ( BROADCASTER_CFG | PERIPHERAL_CFG ) ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;

    #if defined(CTRL_V41_CONFIG) && (CTRL_V41_CONFIG & MST_SLV_CFG)
    case (GAP_PROFILE_CENTRAL | GAP_PROFILE_PERIPHERAL):
      #if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
      {
        stat = SUCCESS;
      }
      #endif
      break;
    #endif // CTRL_V41_CONFIG = MST_SLV_CFG
      
    // Invalid profile roles
    default:
      stat = INVALIDPARAMETER;
      break;
  }

  if ( stat == SUCCESS )
  {
    // Setup the device configuration parameters
    stat = GAP_ParamsInit( taskID, profileRole );
    if ( stat == SUCCESS )
    {
      #if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
      {
#ifndef NO_BLE_SECURITY
        GAP_SecParamsInit( pSRK, pSignCounter );
#endif //!NO_BLE_SECURITY
      }
      #endif
      
      // Set IRK GAP Parameter
      GAP_PrivacyInit( pIRK );

      #if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
      {
        if ( (profileRole == GAP_PROFILE_BROADCASTER) ||
             (profileRole == GAP_PROFILE_PERIPHERAL) )
        {
          maxScanResponses = 0; // Can't scan, so no need for responses. Force 0.
        }

        // Initialize GAP Central Device Manager
        VOID GAP_CentDevMgrInit( maxScanResponses );

        #if ( HOST_CONFIG & CENTRAL_CFG )
        {
          // Register GAP Central Connection processing functions
          GAP_CentConnRegister();

#ifndef NO_BLE_SECURITY
          // Initialize SM Initiator
          VOID SM_InitiatorInit();
#endif //NO_BLE_SECURITY
        }
        #endif
      }
      #endif

      #if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
      {
        // Initialize GAP Peripheral Device Manager
        VOID GAP_PeriDevMgrInit();

        #if ( HOST_CONFIG & PERIPHERAL_CFG )
        {
          // Register GAP Peripheral Connection processing functions
          GAP_PeriConnRegister();
          
#ifndef NO_BLE_SECURITY
          // Initialize SM Responder
          VOID SM_ResponderInit();
#endif //!NO_BLE_SECURITY
        }
        #endif
      }
      #endif
    }
  }

  return ( stat );
}

/*********************************************************************
*********************************************************************/
