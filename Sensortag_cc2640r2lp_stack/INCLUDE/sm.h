/******************************************************************************

 @file  sm.h

 @brief BLE SM API
        
        This file contains the interface to the SM.

 Group: WCS, BTS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2009-2016, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 Release Name: ti-ble-3.0-stack-sdk_3_00_00
 Release Date: 2016-12-21 12:44:48
 *****************************************************************************/

#ifndef SM_H
#define SM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"

#include "hci.h"
  
/*-------------------------------------------------------------------
 * MACROS
 */

/*-------------------------------------------------------------------
 * CONSTANTS
 */
/** @defgroup SM_IO_CAP_DEFINES SM I/O Capabilities
 * @{
 */
#define DISPLAY_ONLY              0x00  //!< Display Only Device
#define DISPLAY_YES_NO            0x01  //!< Display and Yes and No Capable
#define KEYBOARD_ONLY             0x02  //!< Keyboard Only
#define NO_INPUT_NO_OUTPUT        0x03  //!< No Display or Input Device
#define KEYBOARD_DISPLAY          0x04  //!< Both Keyboard and Display Capable
/** @} End SM_IO_CAP_DEFINES */

#define SM_AUTH_MITM_MASK(a)    (((a) & 0x04) >> 2)

/** @defgroup SM_PASSKEY_TYPE_DEFINES SM Passkey Types (Bit Masks)
 * @{
 */
#define SM_PASSKEY_TYPE_INPUT   0x01    //!< Input the passkey
#define SM_PASSKEY_TYPE_DISPLAY 0x02    //!< Display the passkey
/** @} End SM_PASSKEY_TYPE_DEFINES */

/** @defgroup SM_BONDING_FLAGS_DEFINES SM AuthReq Bonding Flags
 * Bonding flags 0x02 and 0x03 are reserved.
 * @{
 */
#define SM_AUTH_REQ_NO_BONDING    0x00  //!< No bonding
#define SM_AUTH_REQ_BONDING       0x01  //!< Bonding
/** @} End SM_BONDING_FLAGS_DEFINES */

#define PASSKEY_LEN     6   //! Passkey Character Length (ASCII Characters)

#define SM_AUTH_STATE_AUTHENTICATED       0x04  //! Authenticate requested
#define SM_AUTH_STATE_BONDING             0x01  //! Bonding requested
#define SM_AUTH_STATE_SECURECONNECTION    0x08  //! Secure Connection requested
   
#define SM_ECC_KEY_LEN 32  // ECC Key length in bytes
   
/* SM private/public key regeneration policy */
#define SM_ECC_KEYS_NOT_AVAILABLE     0xFF  // Initial state of recycled keys before they exist.
#define SM_ECC_KEYS_REGNENERATE_NEVER 0xFF  // Never regenerate the keys.
#define SM_ECC_KEYS_REGENERATE_ALWAYS 0x00  // Always regenerate the keys.
   
/** @defgroup SM_MESSAGE_EVENT_OPCODES SM Message opcocdes
 * @{
 */
#define SM_ECC_KEYS_EVENT 0x00 // ECC Keys
#define SM_DH_KEY_EVENT   0x01 // Diffie-Hellman key

/*-------------------------------------------------------------------
 * General TYPEDEFS
 */

/**
 * SM_NEW_RAND_KEY_EVENT message format.  This message is sent to the
 * requesting task.
 */
typedef struct
{
  osal_event_hdr_t  hdr;      //!< SM_NEW_RAND_KEY_EVENT and status
  uint8 newKey[KEYLEN];       //!< New key value - if status is SUCCESS
} smNewRandKeyEvent_t;

/**
 * header type for ECC and ECDH commands
 */
typedef struct
{
  osal_event_hdr_t hdr;
  uint8 opcode;
} smEventHdr_t;

/**
 * SM_ECC_KEYS_EVENT message format for ECC keys.  This message is sent to 
 * the request task.
 */
typedef struct
{
  osal_event_hdr_t hdr;             //!< SM_MSG_EVENT and status
  uint8 opcode;                     //!< SM_ECC_KEYS_EVENT
  uint8 privateKey[SM_ECC_KEY_LEN]; //!< ECC private key.
  uint8 publicKeyX[SM_ECC_KEY_LEN]; //!< ECC public key X-coordinate.
  uint8 publicKeyY[SM_ECC_KEY_LEN]; //!< ECC public key Y-coordinate.
} smEccKeysEvt_t;

/**
 * SM_DH_KEY_EVENT message format for ECDH keys.  
 * This message is sent to the request task.
 */
typedef struct
{
  osal_event_hdr_t hdr;        //!< SM_MSG_EVENT and status
  uint8 opcode;                //!< SM_DH_KEY_EVENT
  uint8 dhKey[SM_ECC_KEY_LEN]; //!< ECC Diffie-Hellman key
} smDhKeyEvt_t;

/**
 * Key Distribution field  - True or False fields
 */
typedef struct
{
  unsigned int sEncKey:1;    //!< Set to distribute slave encryption key
  unsigned int sIdKey:1;     //!< Set to distribute slave identity key
  unsigned int sSign:1;      //!< Set to distribute slave signing key
  unsigned int sLinkKey:1;   //!< Set to derive slave link key from slave LTK
  unsigned int sReserved:4;  //!< Reserved for slave - don't use
  unsigned int mEncKey:1;    //!< Set to distribute master encryption key
  unsigned int mIdKey:1;     //!< Set to distribute master identity key
  unsigned int mSign:1;      //!< Set to distribute master signing key
  unsigned int mLinkKey:1;   //!< Set to derive master link key from master LTK
  unsigned int mReserved:4;  //!< Reserved for master - don't use
} keyDist_t;

/**
 * ECC keys for pairing.
 */
typedef struct
{
  uint8 isUsed;               //!< FALSE if not used.  USE_PKEYS if public keys only.  USE_ALL_KEYS if public and private keys.
  uint8 sK[SM_ECC_KEY_LEN];   //!< private key (only used if supplied by Bond Manager)
  uint8 pK_x[SM_ECC_KEY_LEN]; //!< public key X-coordinate
  uint8 pK_y[SM_ECC_KEY_LEN]; //!< public key Y-coordinate
} smEccKeys_t;

/**
 * Link Security Requirements
 */
typedef struct
{
  uint8 ioCaps;                 //!< I/O Capabilities (ie.
  uint8 oobAvailable;           //!< True if remote Out-of-band key available
  uint8 oob[KEYLEN];            //!< Out-Of-Bounds key from remote device
  uint8 oobConfirm[KEYLEN];     //!< Out-Of-Bounds confirm from remote device. Secure Connections only.
  uint8 localOobAvailable;      //!< True if local Out-of-band key available. Secure Connections only.
  uint8 localOob[KEYLEN];       //!< Out-Of-Bounds local data. Secure Connections only.
  uint8 isSCOnlyMode;           //!< TRUE if Secure Connections Only Mode. Secure Connections only.
  smEccKeys_t eccKeys;          //!< Optionally specified ECC keys for pairing. Secure Connections only.
  uint8 authReq;                //!< Authentication Requirements
  keyDist_t keyDist;            //!< Key Distribution mask
  uint8 maxEncKeySize;          //!< Maximum Encryption Key size (7-16 bytes)
} smLinkSecurityReq_t;

/**
 * Link Security Information
 */
typedef struct
{
  uint8 ltk[KEYLEN];              //!< Long Term Key (LTK)
  uint16 div;                     //!< LTK Diversifier
  uint8 rand[B_RANDOM_NUM_SIZE];  //!< LTK random number
  uint8 keySize;                  //!< LTK Key Size (7-16 bytes)
} smSecurityInfo_t;

/**
 * Link Identity Information
 */
typedef struct
{
  uint8 irk[KEYLEN];          //!< Identity Resolving Key (IRK)
  uint8 addrType;             //!< Address type for BD_ADDR
  uint8 bd_addr[B_ADDR_LEN];  //!< The advertiser may set this to zeroes to not disclose its BD_ADDR (public address).
} smIdentityInfo_t;

/**
 * Signing Information
 */
typedef struct
{
  uint8  srk[KEYLEN]; //!< Signature Resolving Key (CSRK)
  uint32 signCounter; //!< Sign Counter
} smSigningInfo_t;

/**
 * Pairing Request & Response - authReq field
 */
typedef struct
{
  unsigned int bonding:2;    //!< Bonding flags
  unsigned int mitm:1;       //!< Man-In-The-Middle (MITM)
  unsigned int sc:1;         //!< LE Secure Connections (SC)
  unsigned int kp:1;         //!< LE SC Keypress Notifications
  unsigned int reserved:3;   //!< Reserved - don't use
} authReq_t;

/**
 * Application callback to the SM for ECC keys and Diffie-Hellman Shared Secret (ECDH).
 */
typedef void (*smCB_t)
(
uint8 *secretKey,  //!< Private key when ECC keys were requested.  ECDH shared secret when DHKey is requested.
uint8 *publicKeyX, //!< Local Public Key X-Coordinate.  Not used when DHKey is returned.
uint8 *publicKeyY  //!< Local Public Key Y-Coordinate when ECC keys were requested.  Not used when DHKey is returned.
);

/*-------------------------------------------------------------------
 * GLOBAL VARIABLES
 */

/**
 * @defgroup SM_API Security Manager API Functions
 *
 * @{
 */

/*-------------------------------------------------------------------
 * FUNCTIONS - APPLICATION API - Functions the application may call
 */

/*********************************************************************
 * @fn      SM_RegisterTask
 *
 * @brief   Register Task with the Security Manager to receive
 *          ECC keys or a ECDH shared secret.
 *
 * @params  taskID - Application's task ID.
 *
 * @return  None
 */
extern void SM_RegisterTask( uint8 taskID );

/******************************************************************************
 * @brief  Get ECC private and public keys.  Keys are returned through the
 *         callback registered in SM_RegisterCb.
 *
 * @param  None
 *
 * @return SUCCESS if processing, FAILURE if SM is pairing or 
 *         HCI_ERROR_CODE_MEM_CAP_EXCEEDED if out of heap memory.
 */
extern bStatus_t SM_GetEccKeys( void );

/*********************************************************************
 * @brief   Request a Diffie-Hellman key from the ECC engine. Key is returned 
 *          through the callback registered in SM_RegisterCb, in the first 
 *          parameter.
 *
 * @param   secretKey - the local private key
 * @param   publicKey1 - the remote public key, X-Coordinate.
 * @param   publicKey2 - the remote public key, Y-Coordinate.
 *
 * @return SUCCESS if processing, FAILURE if SM is pairing, or 
 *         HCI_ERROR_CODE_MEM_CAP_EXCEEDED if out of heap memory.
 */
extern bStatus_t SM_GetDHKey( uint8 *secretKey, uint8 *publicKeyX, 
                              uint8 *publicKeyY );

/*********************************************************************
 * @brief   Generate a confirm value for OOB data.
 *
 * @param   publicKey - the local public key X-coordinate
 * @param   oob       - the random string to be sent to the remote device via 
 *                      oob
 * @param   pOut      - pointer to the generated confirm value.  The calling
 *                      application must copy this data into its own buffer.
 *
 * @return  SUCCESS, INVALIDPARAMETER or bleMemAllocError
 */
extern bStatus_t SM_GetScConfirmOob( uint8 *publicKey, uint8 *oob, uint8 *pOut );

/*-------------------------------------------------------------------
 * FUNCTIONS - MASTER API - Only use these in a master device
 */

/**
 * @brief       Initialize SM Initiator on a master device.
 *
 * @return      SUCCESS
 */
extern bStatus_t SM_InitiatorInit( void );

/**
 * @brief       Start the pairing process.  This function is also
 *              called if the device is already bound.
 *
 * NOTE:        Only one pairing process at a time per device.
 *
 * @param       initiator - TRUE to start pairing as Initiator.
 * @param       taskID - task ID to send results.
 * @param       connectionHandle - Link's connection handle
 * @param       pSecReqs - Security parameters for pairing
 *
 * @return      SUCCESS,<BR>
 *              INVALIDPARAMETER,<BR>
 *              bleAlreadyInRequestedMode
 */
extern bStatus_t SM_StartPairing(  uint8 initiator,
                                   uint8 taskID,
                                   uint16 connectionHandle,
                                   smLinkSecurityReq_t *pSecReqs );

/**
 * @brief       Send Start Encrypt through HCI
 *
 * @param       connHandle - Connection Handle
 * @param       pLTK - pointer to 16 byte lkt
 * @param       div - div or ediv
 * @param       pRandNum - pointer to 8 byte random number
 * @param       keyLen - length of LTK (bytes)
 *
 * @return      SUCCESS,<BR>
 *              INVALIDPARAMETER,<BR>
 *              other from HCI/LL
 */
extern bStatus_t SM_StartEncryption( uint16 connHandle, uint8 *pLTK,
                                     uint16 div, uint8 *pRandNum, 
                                     uint8 keyLen );


/*-------------------------------------------------------------------
 * FUNCTIONS - SLAVE API - Only use these in a slave device
 */

/**
 * @brief       Initialize SM Responder on a slave device.
 *
 * @return      SUCCESS
 */
extern bStatus_t SM_ResponderInit( void );

/*-------------------------------------------------------------------
 * FUNCTIONS - GENERAL API - both master and slave
 */

/**
 * @brief       Generate a key with a random value.
 *
 * @param       taskID - task ID to send results.
 *
 * @return      SUCCESS,<BR>
 *              bleNotReady,<BR>
 *              bleMemAllocError,<BR>
 *              FAILURE
 */
extern bStatus_t SM_NewRandKey( uint8 taskID );

#if !defined (BLE_V42_FEATURES) || !(BLE_V42_FEATURES & PRIVACY_1_2_CFG)
/**
 * @brief       Calculate a new Private Resolvable address.
 *
 * @param       pIRK - Identity Root Key.
 * @param       pNewAddr - pointer to place to put new calc'd address
 *
 * @return      SUCCESS - if started,<BR>
 *              INVALIDPARAMETER
 */
extern bStatus_t SM_CalcRandomAddr( uint8 *pIRK, uint8 *pNewAddr );

/**
 * @brief       Resolve a Private Resolvable Address.
 *
 * @param       pIRK - pointer to the IRK
 * @param       pAddr - pointer to the random address
 *
 * @return      SUCCESS - match,<BR>
 *              FAILURE - don't match,<BR>
 *              INVALIDPARAMETER - parameters invalid
 */
extern bStatus_t SM_ResolveRandomAddrs( uint8 *pIRK, uint8 *pAddr );
#endif // ! BLE_V42_FEATURES | ! PRIVACY_1_2_CFG

/**
 * @brief       Encrypt the plain text data with the key..
 *
 * @param       pKey - key data
 * @param       pPlainText - Plain text data
 * @param       pResult - place to put the encrypted result
 *
 * @return      SUCCESS - if started,<BR>
 *              INVALIDPARAMETER - one of the parameters are NULL,<BR>
 *              bleAlreadyInRequestedMode,<BR>
 *              bleMemAllocError
 */
extern bStatus_t SM_Encrypt( uint8 *pKey, uint8 *pPlainText, uint8 *pResult );

/**
 * @brief       Generate an outgoing Authentication Signature.
 *
 * @param       pData - message data
 * @param       len - length of pData
 * @param       pAuthenSig - place to put new signature
 *
 * @return      SUCCESS - signature authentication generated,<BR>
 *              INVALIDPARAMETER - pData or pAuthenSig is NULL,<BR>
 *              bleMemAllocError
 */
extern bStatus_t SM_GenerateAuthenSig( uint8 *pData, uint16 len, 
                                       uint8 *pAuthenSig );

/**
 * @brief       Verify an Authentication Signature.
 *
 * @param       connHandle - connection to verify against.
 * @param       authentication - TRUE if requires an authenticated CSRK, FALSE if not
 * @param       pData - message data
 * @param       len - length of pData
 * @param       pAuthenSig - message signature to verify
 *
 * @return      SUCCESS - signature authentication verified,<BR>
 *              FAILURE - if not verified,<BR>
 *              bleNotConnected - Connection not found,<BR>
 *              INVALIDPARAMETER - pData or pAuthenSig is NULL, or signCounter is invalid,<BR>
 *              bleMemAllocError
 */
extern bStatus_t SM_VerifyAuthenSig( uint16 connHandle,
                                     uint8 authentication,
                                     uint8 *pData,
                                     uint16 len,
                                     uint8 *pAuthenSig );

/**
 * @brief       Update the passkey for the pairing process.
 *
 * @param       pPasskey - pointer to the 6 digit passkey
 * @param       connectionHandle - connection handle to link.
 *
 * @return      SUCCESS,<BR>
 *              bleIncorrectMode - Not pairing,<BR>
 *              INVALIDPARAMETER - link is incorrect
 */
extern bStatus_t SM_PasskeyUpdate( uint8 *pPasskey, uint16 connectionHandle );


/**
 * @brief       Update the regeneration count for ECC key re-usage.
 *
 * @param       count - number of times a keypair can be reused before regeneration.
 *
 * @return      None
 */
extern void SM_SetECCRegenerationCount(uint8 count);


/**
 * @} End SM_API
 */

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by OSAL.
 */

  /**
   * @internal
   *
   * @brief       SM Task Initialization Function.
   *
   * @param       taskID - SM task ID.
   *
   * @return      void
   */
  extern void SM_Init( uint8 task_id );

  /**
   * @internal
   *
   * @brief       SM Task event processing function.
   *
   * @param       taskID - SM task ID
   * @param       events - SM events.
   *
   * @return      events not processed
   */
  extern uint16 SM_ProcessEvent( uint8 task_id, uint16 events );

  /**
   * @internal
   *
   * @fn      SM_p256KeyCB
   *
   * @brief   Read P-256 Key Event Callback from the Controller.  The secret key
   *          used to generate the public key pair is also input here.
   *
   * @params  pK         - pointer to HCI BLE Read P256 Public Key Complete struct
   *                       which contain the X and Y Coordinates of the ECC Public
   *                       Keys.
   *
   * @param   privateKey - the private key used to generated the ECC Public Keys.
   *
   * @return  None
   */
  extern void SM_p256KeyCB( hciEvt_BLEReadP256PublicKeyComplete_t *pK, 
                            uint8 *privateKey );

  /**
   * @internal
   *
   * @fn      SM_dhKeyCB
   *
   * @brief   BLE Generate Diffie-Hellman Key Event Callback from the controller.
   *
   * @params         - pointer to HCI BLE Read P256 Public Key Complete struct
   *                   which contain the X and Y Coordinates of the ECC Public
   *                   Keys.
   *
   * @param   pDhKey - the Diffie-Hellman X and Y Coordinates output from the 
   *                   ECDH operation.
   *
   * @return  None
   */
  extern void SM_dhKeyCB( hciEvt_BLEGenDHKeyComplete_t *pDhKey );

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SM_H */
