/******************************************************************************

 @file  sensortag_lp.c

 @brief This file is the SensorTag application's main body.

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


/*******************************************************************************
 * INCLUDES
 */
#include <string.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>

#include <ICall.h>

#include "hci_tl.h"
#include "gatt.h"
#include "linkdb.h"
#include "gapgattserver.h"
#include "gattservapp.h"
#include "gatt_profile_uuid.h"
#include "gapbondmgr.h"
#include "osal_snv.h"
#include "util.h"
#include "icall_apimsg.h"

#include "board_key.h"
#include "board.h"

#include "sensortag.h"
#include "sensortag_revision.h"
#include "devinfoservice.h"
#include "proxreporter.h"
#include "accelerometer.h"
#include "peripheral.h"
#include "gapbondmgr.h"
#include "simplekeys.h"
#include "st_util.h"
#include "bma250.h"

#if defined(FEATURE_OAD) || defined(IMAGE_INVALIDATE)
#include "oad_target.h"
#include "oad.h"
#endif //FEATURE_OAD || IMAGE_INVALIDATE

#include "sensortag_register.h"

// On-board devices
#include "sensortag_keys.h"
#include "sensortag_io.h"
#include "sensortag_batt.h"
#include "sensortag_buzzer.h"
#include "sensortag_conn_ctrl.h"

#include "icall_api.h"

/*******************************************************************************
 * CONSTANTS
 */

//keyfob add
// Number of beeps before buzzer stops by itself
#define BUZZER_MAX_BEEPS                      5

// Buzzer beep tone frequency for "High Alert" (in Hz)
#define BUZZER_ALERT_HIGH_FREQ                2000

// Buzzer beep tone frequency for "Low Alert" (in Hz)
#define BUZZER_ALERT_LOW_FREQ                 1200

// How often (in ms) to read the accelerometer
#define ACCEL_READ_PERIOD                     50

// Minimum change in accelerometer before sending a notification
#define ACCEL_CHANGE_THRESHOLD                5
//from keyfob end

// How often to perform periodic event (in milliseconds)
#define ST_PERIODIC_EVT_PERIOD               1000

// What is the advertising interval when device is discoverable
// (units of 625us, 160=100ms)
#define DEFAULT_ADVERTISING_INTERVAL          160

// General discoverable mode advertises indefinitely
#define DEFAULT_DISCOVERABLE_MODE             GAP_ADTYPE_FLAGS_LIMITED

#ifndef FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 80=100ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms, 800=1000ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     800
#else //!FEATURE_OAD
// Minimum connection interval (units of 1.25ms, 8=10ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL     8

// Maximum connection interval (units of 1.25ms, 8=10ms) if automatic
// parameter update request is enabled
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL     8
#endif //FEATURE_OAD

// Slave latency to use if automatic parameter update request is enabled
#define DEFAULT_DESIRED_SLAVE_LATENCY         0

// Supervision timeout value (units of 10ms, 1000=10s) if automatic parameter
// update request is enabled
#define DEFAULT_DESIRED_CONN_TIMEOUT          1000 // 10 s default 1

// Whether to enable automatic parameter update request when a
// connection is formed
#define DEFAULT_ENABLE_UPDATE_REQUEST         GAPROLE_LINK_PARAM_UPDATE_INITIATE_BOTH_PARAMS

// Connection Pause Peripheral time value (in seconds)
#define DEFAULT_CONN_PAUSE_PERIPHERAL         5 //1 default

// Advertising after initialization or due to terminated link
#define ST_PROXSTATE_INITIALIZED              0

// Connected and "within range" of the master, as defined by proximity profile
#define ST_PROXSTATE_CONNECTED_IN_RANGE       1

// Connected and "out of range" of the master, as defined by proximity profile
#define ST_PROXSTATE_PATH_LOSS                2

// Disconnected as a result of a supervision timeout
#define ST_PROXSTATE_LINK_LOSS                3

// buzzer_state values
#define BUZZER_OFF                            0
#define BUZZER_ON                             1

// Company Identifier: Texas Instruments Inc. (13)
#define TI_COMPANY_ID                         0x000D
#define TI_ST_DEVICE_ID                       0x03
#define TI_ST_KEY_DATA_ID                     0x00

// Length of board address
#define B_ADDR_STR_LEN                        15

#if defined (PLUS_BROADCASTER)
  #define ADV_IN_CONN_WAIT                    500 // delay 500 ms
#endif

// Task configuration
#define ST_TASK_PRIORITY                      1

#ifndef ST_TASK_STACK_SIZE
// Stack size may be overridden by project settings
#define ST_TASK_STACK_SIZE                    700
#endif

// Misc.
#define INVALID_CONNHANDLE                    0xFFFF
#define TEST_INDICATION_BLINKS                5  // Number of blinks
#define OAD_PACKET_SIZE                       18
#define KEY_STATE_OFFSET                      13 // Offset in advertising data

/*******************************************************************************
 * TYPEDEFS
 */

// App event passed from profiles.
typedef struct
{
  uint8_t event;  // Which profile's event
  uint8_t serviceID; // New status
  uint8_t paramID;
} stEvt_t;

/*******************************************************************************
 * GLOBAL VARIABLES
 */
// Profile state and parameters
gaprole_States_t gapProfileState = GAPROLE_INIT;

// Entity ID globally used to check for source and/or destination of messages
ICall_EntityID selfEntityMain;

// Event globally used to post local events and pend on system and
// local events.
ICall_SyncHandle syncEvent;

// Global pin resources
PIN_State pinGpioState;
PIN_Handle hGpioPin;
uint32_t events;

// Globals used for ATT Response retransmission added from Project Zero
static gattMsgEvent_t *pAttRsp = NULL;
static uint8_t rspTxRetry = 0;

/*******************************************************************************
 * LOCAL VARIABLES
 */

// Task configuration
static Task_Struct sensorTagTask;
static Char sensorTagTaskStack[ST_TASK_STACK_SIZE];

// Clock instances for internal periodic events.
static Clock_Struct periodicClock;
static Clock_Struct accelReadClock;
static Clock_Struct toggleBuzzerClock;

// Proximity State Variables from Key Fob
static uint8_t sensortagProxLLAlertLevel = PP_ALERT_LEVEL_NO; // Link Loss Alert
static uint8_t sensortagProxIMAlertLevel = PP_ALERT_LEVEL_NO; // Immediate Alert
static int8_t  sensortagProxTxPwrLevel = 0;  // Tx Power Level (0dBm default)

// sensortagProximityState is the current state of the device
static uint8_t sensortagProximityState;

uint8_t sensortagAlertState;

// Queue object used for app messages
static Queue_Struct appMsg;
static Queue_Handle appMsgQueue;

// self-test result
static uint8_t selfTestMap;

// GAP - SCAN RSP data (max size = 31 bytes)
static uint8_t scanRspData[] =
{
  // complete name
  0x14,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
#ifdef CC1350_LAUNCHXL
  'C', 'C', '1', '3', '5', '0', ' ',
#else
  'C', 'C', '2', '6', '4', '0', 'R',
#endif
  '2', 'F', ' ', 'S', 'e', 'n', 's',
  'o', 'r', 't', 'a', 'g',
  // connection interval range
  0x05,   // length of this data
  GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
  LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
  LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),
  HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

  // Tx power level
  0x02,   // length of this data
  GAP_ADTYPE_POWER_LEVEL,
  0       // 0dBm
};

// GAP - Advertisement data (max size = 31 bytes, though this is
// best kept short to conserve power while advertising)
static uint8_t advertData[] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  DEFAULT_DISCOVERABLE_MODE | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

  // service UUID, to notify central devices what services are included
  // in this peripheral
#if !defined(FEATURE_OAD) || defined(FEATURE_OAD_ONCHIP)
  0x09,   // length of this data
#else
  0x05,
#endif
  GAP_ADTYPE_16BIT_MORE,      // some of the UUID's, but not all
  LO_UINT16(LINK_LOSS_SERV_UUID),       // Link Loss Service (Proximity Profile)
  HI_UINT16(LINK_LOSS_SERV_UUID),
  LO_UINT16(IMMEDIATE_ALERT_SERV_UUID), // Immediate Alert Service (Proximity/Find Me Profile)
  HI_UINT16(IMMEDIATE_ALERT_SERV_UUID),
  LO_UINT16(TX_PWR_LEVEL_SERV_UUID),    // Tx Power Level Service (Proximity Profile)
  HI_UINT16(TX_PWR_LEVEL_SERV_UUID),
#ifdef FEATURE_OAD
  LO_UINT16(DISPLAY_SERV_UUID),
  HI_UINT16(DISPLAY_SERV_UUID),
#endif //FEATURE_OAD
#ifndef FEATURE_OAD_ONCHIP
  LO_UINT16(SK_SERV_UUID),
  HI_UINT16(SK_SERV_UUID),
#endif

  // Manufacturer specific advertising data
  0x06,
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,
  LO_UINT16(TI_COMPANY_ID),
  HI_UINT16(TI_COMPANY_ID),
  TI_ST_DEVICE_ID,
  TI_ST_KEY_DATA_ID,
  0x00                                    // Key state
};

// Device information parameters
#ifdef CC1350_LAUNCHXL
static const uint8_t devInfoModelNumber[] = "CC1350 LaunchPad";
#else
static const uint8_t devInfoModelNumber[] = "CC2640R2 Sensortag";
#endif
static const uint8_t devInfoNA[] =          "N.A.";
static const uint8_t devInfoFirmwareRev[] = FW_VERSION_STR;
static const uint8_t devInfoMfrName[] =     "Texas Instruments";
static const uint8_t *devInfoHardwareRev =  devInfoNA;

// GAP GATT Attributes
static const uint8_t *attDeviceName = devInfoModelNumber;

// Buzzer state
static uint8_t buzzer_state = BUZZER_OFF;
static uint8_t buzzer_beep_count = 0;

// Accelerometer Profile Parameters
static uint8_t accelEnabler = FALSE;

// Pins that are actively used by the application
static PIN_Config SensortagAppPinTable[] =
{
#ifdef CC2640R2_LAUNCHXL
    Board_GLED  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,     /* LED initially off      */
    Board_RLED  | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,     /* LED initially off      */
#endif
#ifdef SENSORTAG_CC2640R2_LAUNCHXL
    BP_BLED   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,         /* LED initially off      */
    BP_GLED   | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,         /* LED initially off      */
    BP_BUZZER | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,         /* Board Buzzer           */
#endif
    Board_BTN1  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_BOTHEDGES | PIN_HYSTERESIS,          /* Button is active low   */
    Board_BTN2  | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_BOTHEDGES | PIN_HYSTERESIS,          /* Button is active low   */
    PIN_TERMINATE
};

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

static void SensorTag_init(void);
static void SensorTag_taskFxn(UArg a0, UArg a1);
static uint8_t SensorTag_processStackMsg(ICall_Hdr *pMsg);
static uint8_t SensorTag_processGATTMsg(gattMsgEvent_t *pMsg);
static void SensorTag_processAppMsg(stEvt_t *pMsg);
static void SensorTag_processStateChangeEvt(gaprole_States_t newState) ;
static void SensorTag_processCharValueChangeEvt(uint8_t serviceID, uint8_t paramID) ;
static void SensorTag_performPeriodicTask(void);
static void SensorTag_stateChangeCB(gaprole_States_t newState);
static void SensorTag_sendAttRsp(void);
static void SensorTag_freeAttRsp(uint8_t status);

#ifndef FEATURE_OAD_ONCHIP
void SensorTag_charValueChangeCB(uint8_t serviceID, uint8_t paramID);
#endif //!FEATURE_OAD_ONCHIP

static void SensorTag_resetAllModules(void);
static void SensorTag_clockHandler(UArg arg);
static void SensorTag_enqueueMsg(uint8_t event, uint8_t serviceID, uint8_t paramID);
static void SensorTag_callback(PIN_Handle handle, PIN_Id pinId);
static void SensorTag_setDeviceInfo(void);

static void SensorTag_performAlert(void);
static void SensorTag_proximityAttrCB(uint8_t attrParamID);
static void SensorTag_processProximityAttrEvt(uint8_t attrParamID);
static void SensorTag_processToggleBuzzerEvt(void);
static void SensorTag_accelEnablerChangeCB(void);
static void SensorTag_processAccelEnablerChangeEvt(void);
static void SensorTag_processAccelReadEvt(void);
static void SensorTag_accelRead(void);

/*******************************************************************************
 * PROFILE CALLBACKS
 */

// GAP Role Callbacks
static gapRolesCBs_t sensorTag_gapRoleCBs =
{
  SensorTag_stateChangeCB     // Profile State Change Callbacks
};

// GAP Bond Manager Callbacks
static gapBondCBs_t sensorTag_bondMgrCBs =
{
  NULL, // Passcode callback (not used by application)
  NULL  // Pairing / Bonding state Callback (not used by application)
};

// Proximity Peripheral Profile Callbacks
static proxReporterCBs_t SensorTag_proximityCBs =
{
  SensorTag_proximityAttrCB  // Whenever the Link Loss Alert attribute changes
};

// Accelerometer Profile Callbacks
static accelCBs_t SensorTag_accelCBs =
{
  SensorTag_accelEnablerChangeCB  // Called when Enabler attribute changes
};
/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

/*******************************************************************************
 * @fn      SensorTag_createTask
 *
 * @brief   Task creation function for the SensorTag.
 *
 * @param   none
 *
 * @return  none
 */
void SensorTag_createTask(void)
{
  Task_Params taskParams;

  // Configure task
  Task_Params_init(&taskParams);
  taskParams.stack = sensorTagTaskStack;
  taskParams.stackSize = ST_TASK_STACK_SIZE;
  taskParams.priority = ST_TASK_PRIORITY;

  Task_construct(&sensorTagTask, SensorTag_taskFxn, &taskParams, NULL);
}

/*******************************************************************************
 * @fn      SensorTag_testResult
 *
 * @brief   Get result of self-test
 *
 * @param   none
 *
 * @return  bitmap with result of self-test
 */
uint8_t SensorTag_testResult(void)
{
    //selfTestMap = 0; //temporary only

    return selfTestMap;
}

/*******************************************************************************
 * @fn      SensorTag_init
 *
 * @brief   Called during initialization and contains application
 *          specific initialization (i.e. hardware initialization/setup,
 *          table initialization, power up notification, etc), and
 *          profile initialization/setup.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_init(void)
{
  // Handling of buttons, LED, relay
  hGpioPin = PIN_open(&pinGpioState, SensortagAppPinTable);
  PIN_registerIntCb(hGpioPin, SensorTag_callback);

  // ***************************************************************************
  // N0 STACK API CALLS CAN OCCUR BEFORE THIS CALL TO ICall_registerApp
  // ***************************************************************************
  // Register the current thread as an ICall dispatcher application
  // so that the application can send and receive messages.
  ICall_registerApp(&selfEntityMain, &syncEvent);

  // added address
  // Hard code the DB Address till CC2650 board gets its own IEEE address
  uint8 bdAddress[B_ADDR_LEN] = { 0x22, 0x22, 0x22, 0x22, 0x22, 0x5A };
  //uint8 bdAddress[B_ADDR_LEN] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
  HCI_EXT_SetBDADDRCmd(bdAddress);

  // Set device's Sleep Clock Accuracy
  //HCI_EXT_SetSCACmd(40);

  // Create an RTOS queue for message from profile to be sent to app.
  appMsgQueue = Util_constructQueue(&appMsg);

  // Create one-shot clocks for internal periodic events.
  Util_constructClock(&periodicClock, SensorTag_clockHandler,
                      ST_PERIODIC_EVT_PERIOD, 0, false, ST_PERIODIC_EVT);

  //From keyfob
  Util_constructClock(&accelReadClock, SensorTag_clockHandler,
                      ACCEL_READ_PERIOD, 0, false, ST_ACCEL_READ_EVT);

  Util_constructClock(&toggleBuzzerClock, SensorTag_clockHandler,
                      200, 800, false, ST_TOGGLE_BUZZER_EVT);
  //end keyfob

  // Setup the GAP
  GAP_SetParamValue(TGAP_CONN_PAUSE_PERIPHERAL, DEFAULT_CONN_PAUSE_PERIPHERAL);

  // Setup the GAP Peripheral Role Profile
  {
    // For all hardware platforms, device starts advertising upon initialization
    uint8_t initialAdvertEnable = TRUE;

    // By setting this to zero, the device will go into the waiting state after
    // being discoverable for 30.72 second, and will not being advertising again
    // until the enabler is set back to TRUE
    uint16_t advertOffTime = 0;

    uint8_t enableUpdateRequest = DEFAULT_ENABLE_UPDATE_REQUEST;
    uint16_t desiredMinInterval = DEFAULT_DESIRED_MIN_CONN_INTERVAL;
    uint16_t desiredMaxInterval = DEFAULT_DESIRED_MAX_CONN_INTERVAL;
    uint16_t desiredSlaveLatency = DEFAULT_DESIRED_SLAVE_LATENCY;
    uint16_t desiredConnTimeout = DEFAULT_DESIRED_CONN_TIMEOUT;

    // Set the GAP Role Parameters
    GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t),
                         &initialAdvertEnable);
    GAPRole_SetParameter(GAPROLE_ADVERT_OFF_TIME, sizeof(uint16_t),
                         &advertOffTime);

    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData),
                         scanRspData);
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);

    GAPRole_SetParameter(GAPROLE_PARAM_UPDATE_ENABLE, sizeof(uint8_t),
                         &enableUpdateRequest);
    GAPRole_SetParameter(GAPROLE_MIN_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMinInterval);
    GAPRole_SetParameter(GAPROLE_MAX_CONN_INTERVAL, sizeof(uint16_t),
                         &desiredMaxInterval);
    GAPRole_SetParameter(GAPROLE_SLAVE_LATENCY, sizeof(uint16_t),
                         &desiredSlaveLatency);
    GAPRole_SetParameter(GAPROLE_TIMEOUT_MULTIPLIER, sizeof(uint16_t),
                         &desiredConnTimeout);
  }

  // Set the GAP Characteristics
  GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN,
                   (void*)attDeviceName);

  // Set advertising interval
  {
    uint16_t advInt = DEFAULT_ADVERTISING_INTERVAL;

    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_LIM_DISC_ADV_INT_MAX, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MIN, advInt);
    GAP_SetParamValue(TGAP_GEN_DISC_ADV_INT_MAX, advInt);
  }

  // Setup the GAP Bond Manager
  {
    uint32_t passkey = 0; // passkey "000000"
    uint8_t pairMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
    uint8_t mitm = FALSE;
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t bonding = TRUE;

    GAPBondMgr_SetParameter(GAPBOND_DEFAULT_PASSCODE, sizeof(uint32_t),
                            &passkey);
    GAPBondMgr_SetParameter(GAPBOND_PAIRING_MODE, sizeof(uint8_t), &pairMode);
    GAPBondMgr_SetParameter(GAPBOND_MITM_PROTECTION, sizeof(uint8_t), &mitm);
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetParameter(GAPBOND_BONDING_ENABLED, sizeof(uint8_t), &bonding);
  }

   // Initialize GATT attributes
  GGS_AddService(GATT_ALL_SERVICES);              // GAP
  GATTServApp_AddService(GATT_ALL_SERVICES);      // GATT attributes
  DevInfo_AddService();                           // Add device info service.
  ProxReporter_AddService(GATT_ALL_SERVICES);     // Proximity Reporter Profile
  Accel_AddService(GATT_ALL_SERVICES);            // Accelerometer Profile

  SensorTag_setDeviceInfo();                      // Add application specific device information
  SensorTagBatt_init();                           // Add battery monitor
  // Auxiliary services
  SensorTagKeys_init();                           // Simple Keys
  SensorTagIO_init();                             // IO (LED+buzzer+self test)
  SensorTagRegister_init();                       // Register Service
  //SensorTagOad_init();                          // Over the Air Download
#ifdef IMAGE_INVALIDATE
  Reset_addService();
#endif //IMAGE_INVALIDATE

  selfTestMap = 0; //no Sensor and ExtFlash test done
  sensortagProximityState = ST_PROXSTATE_INITIALIZED;

  // Initialize Tx Power Level characteristic in Proximity Reporter
  {
    int8_t initialTxPowerLevel = 0;

    ProxReporter_SetParameter(PP_TX_POWER_LEVEL, sizeof (int8_t),
                              &initialTxPowerLevel);
  }

  sensortagAlertState = ALERT_STATE_OFF;

  // Make sure buzzer is off.
  SensorTagBuzzer_close();

  // Start the Proximity Profile
  VOID ProxReporter_RegisterAppCBs(&SensorTag_proximityCBs);

  // Start the Accelerometer Profile
  VOID Accel_RegisterAppCBs(&SensorTag_accelCBs);

  // Set the proximity attribute values to default
  ProxReporter_SetParameter(PP_LINK_LOSS_ALERT_LEVEL, sizeof(uint8_t),
                            &sensortagProxLLAlertLevel);
  ProxReporter_SetParameter(PP_IM_ALERT_LEVEL, sizeof(uint8_t),
                            &sensortagProxIMAlertLevel);
  ProxReporter_SetParameter(PP_TX_POWER_LEVEL, sizeof(int8_t),
                            &sensortagProxTxPwrLevel);

  // Start the Device
  VOID GAPRole_StartDevice(&sensorTag_gapRoleCBs);

  // Start Bond Manager
  VOID GAPBondMgr_Register(&sensorTag_bondMgrCBs);

  // Register with GAP for HCI/Host messages
  GAP_RegisterForMsgs(selfEntityMain); //added by Markel

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(selfEntityMain); //added by Markel

  // Enable interrupt handling for keys and relay
  PIN_registerIntCb(hGpioPin, SensorTag_callback);
}

/*******************************************************************************
 * @fn      SensorTag_taskFxn
 *
 * @brief   Application task entry point for the SensorTag
 *
 * @param   a0, a1 (not used)
 *
 * @return  none
 */
static void SensorTag_taskFxn(UArg a0, UArg a1)
{

  //uint32_t events; // re-declared as extern at sensortag.h
  // Initialize application
  SensorTag_init();

  // Application main loop
  for (;;)
  {

    // Waits for an event to be posted associated with the calling thread.
    // Note that an event associated with a thread is posted when a
    // message is queued to the message receive queue of the thread

    events = Event_pend(syncEvent, Event_Id_NONE, ST_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);

    if(events)
    {
      ICall_EntityID dest;
      ICall_ServiceEnum src;
      ICall_HciExtEvt *pMsg = NULL;

      if (ICall_fetchServiceMsg(&src, &dest,
                                (void **)&pMsg) == ICALL_ERRNO_SUCCESS)
      {
        uint8 safeToDealloc = TRUE;

        if ((src == ICALL_SERVICE_CLASS_BLE) && (dest == selfEntityMain))
        {
            ICall_Stack_Event *pEvt = (ICall_Stack_Event *)pMsg;

            // Check for event flags received (event signature 0xffff)
            if (pEvt->signature == 0xffff)
            {
              // Event received when a connection event is completed
              if (pEvt->event_flag & ST_CONN_EVT_END_EVT)
              {
                  // Try to retransmit pending ATT Response (if any)
                 SensorTag_sendAttRsp();
              }
            }
            else // It's a message from the stack and not an event.
            {
                // Process inter-task message
                safeToDealloc = SensorTag_processStackMsg((ICall_Hdr *)pMsg);
            }
        }

        if (pMsg && safeToDealloc)
        {
          ICall_freeMsg(pMsg);
        }
      }

      // If RTOS queue is not empty, process app message.
      if (events & ST_QUEUE_EVT)
      {
        while (!Queue_empty(appMsgQueue))
        {
          stEvt_t *pMsg = (stEvt_t *)Util_dequeueMsg(appMsgQueue);
          if (pMsg)
          {
            // Process message.
            SensorTag_processAppMsg(pMsg);

            // Free the space from the message.
            ICall_free(pMsg);
          }
        }
      }

      // Process new data if available
      if ((events & SK_KEY_CHANGE_EVT) ||
          (events & SK_EVT_FACTORY_RESET) ||
           (events & SK_EVT_DISCONNECT))
      {
        SensorTagKeys_processEvent();
      }

      if (events & ST_ACCEL_CHANGE_EVT)
      {
        SensorTag_processAccelEnablerChangeEvt();
      }

      if (events & ST_ACCEL_READ_EVT)
      {
        SensorTag_processAccelReadEvt();
      }

      if (events & ST_TOGGLE_BUZZER_EVT)
      {
        SensorTag_processToggleBuzzerEvt();
      }

      if (events & ST_BATTERY_CHECK_EVT)
      {
        SensorTagBatt_processSensorEvent();
      }

      if (!!(events & ST_PERIODIC_EVT))
      {

        if (gapProfileState == GAPROLE_CONNECTED
            || gapProfileState == GAPROLE_ADVERTISING)
        {
          Util_startClock(&periodicClock);
        }

        // Perform periodic application task
        if (gapProfileState == GAPROLE_CONNECTED)
        {
          SensorTag_performPeriodicTask();
        }

        // Blink green LED when advertising
        if (gapProfileState == GAPROLE_ADVERTISING)
        {
          SensorTagIO_blinkLed(IOID_GREEN_LED, 1);
        }
      }
    }
  } // task loop
}


/*******************************************************************************
 * @fn      SensorTag_setDeviceInfo
 *
 * @brief   Set application specific Device Information
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_setDeviceInfo(void)
{
  DevInfo_SetParameter(DEVINFO_MODEL_NUMBER, sizeof(devInfoModelNumber),
                       (void*)devInfoModelNumber);
  DevInfo_SetParameter(DEVINFO_SERIAL_NUMBER, sizeof(devInfoNA),
                       (void*)devInfoNA);
  DevInfo_SetParameter(DEVINFO_SOFTWARE_REV, sizeof(devInfoNA),
                       (void*)devInfoNA);
  DevInfo_SetParameter(DEVINFO_FIRMWARE_REV, sizeof(devInfoFirmwareRev),
                       (void*)devInfoFirmwareRev);
  DevInfo_SetParameter(DEVINFO_HARDWARE_REV, sizeof(devInfoHardwareRev),
                       (void*)devInfoHardwareRev);
  DevInfo_SetParameter(DEVINFO_MANUFACTURER_NAME, sizeof(devInfoMfrName),
                       (void*)devInfoMfrName);
}

/*******************************************************************************
 * @fn      SensorTag_processAppMsg
 *
 * @brief   Process an incoming callback from a profile.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void SensorTag_processAppMsg(stEvt_t *pMsg)
{
  switch (pMsg->event)
  {
    case ST_STATE_CHANGE_EVT:
      SensorTag_processStateChangeEvt((gaprole_States_t)pMsg->serviceID);
      break;

    case ST_CHAR_CHANGE_EVT:
      SensorTag_processCharValueChangeEvt(pMsg->serviceID, pMsg->paramID);
      break;

    default:
      // Do nothing.
      break;
  }
}

/*******************************************************************************
 * @fn      SensorTag_stateChangeCB
 *
 * @brief   Callback from GAP Role indicating a role state change.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SensorTag_stateChangeCB(gaprole_States_t newState)
{
  SensorTag_enqueueMsg(ST_STATE_CHANGE_EVT, newState, NULL);
}

/*******************************************************************************
 * @fn      SensorTag_processStateChangeEvt
 *
 * @brief   Process a pending GAP Role state change event.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void SensorTag_processStateChangeEvt(gaprole_States_t newState)
{
  uint8_t valFalse = FALSE;

  switch (newState)
  {
  case GAPROLE_STARTED:
    {
      uint8_t ownAddress[B_ADDR_LEN];
      uint8_t systemId[DEVINFO_SYSTEM_ID_LEN];

      SensorTagIO_blinkLed(IOID_GREEN_LED, 5);

      GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddress);

      // use 6 bytes of device address for 8 bytes of system ID value
      systemId[0] = ownAddress[0];
      systemId[1] = ownAddress[1];
      systemId[2] = ownAddress[2];

      // set middle bytes to zero
      systemId[4] = 0x00;
      systemId[3] = 0x00;

      // shift three bytes up
      systemId[7] = ownAddress[5];
      systemId[6] = ownAddress[4];
      systemId[5] = ownAddress[3];

      DevInfo_SetParameter(DEVINFO_SYSTEM_ID, DEVINFO_SYSTEM_ID_LEN, systemId);
    }
    break;

  case GAPROLE_ADVERTISING:
    // Start the clock
    if (!Util_isActive(&periodicClock))
    {
      Util_startClock(&periodicClock);
    }

    // Make sure key presses are not stuck
    SensorTag_updateAdvertisingData(0);
    break;

  case GAPROLE_CONNECTED:
    {

      // Set the proximity state to either path loss alert or in range depending
      // on the value of sensortagProxIMAlertLevel (which was set by proximity
      // monitor).
      if(sensortagProxIMAlertLevel != PP_ALERT_LEVEL_NO)
      {
        sensortagProximityState = ST_PROXSTATE_PATH_LOSS;

        // Perform alert.
        SensorTag_performAlert();
        buzzer_beep_count = 0;
      }
      // If sensortagProxIMAlertLevel is PP_ALERT_LEVEL_NO
      else
      {
        sensortagProximityState = ST_PROXSTATE_CONNECTED_IN_RANGE;
        SensorTag_stopAlert();
      }

      //GAPRole_GetParameter(GAPROLE_CONNHANDLE, &connHandle); //evaluate if needed

      // Start the clock
      if (!Util_isActive(&periodicClock))
      {
        Util_startClock(&periodicClock);
      }

      // Turn off LEDs and buzzer
#ifdef CC2640R2_LAUNCHXL
      PIN_setOutputValue(hGpioPin, Board_RLED, Board_LED_OFF);
      PIN_setOutputValue(hGpioPin, Board_GLED, Board_LED_OFF);
#endif
#ifdef SENSORTAG_CC2640R2_LAUNCHXL
      PIN_setOutputValue(hGpioPin, BP_BLED, Board_LED_OFF);
      PIN_setOutputValue(hGpioPin, BP_GLED, Board_LED_OFF);
#endif

      SensorTagConnectionControl_update(); //currently EXCLUDE_OAD
    }
    break;

  case GAPROLE_CONNECTED_ADV:
    break;

  case GAPROLE_WAITING:
    {
      // Then the link was terminated intentionally by the slave or master,
      // or advertising timed out
      sensortagProximityState = ST_PROXSTATE_INITIALIZED;

      // Turn off immediate alert.
      ProxReporter_SetParameter(PP_IM_ALERT_LEVEL, sizeof(valFalse), &valFalse);
      sensortagProxIMAlertLevel = PP_ALERT_LEVEL_NO;

      // Change attribute value of Accelerometer Enable to FALSE.
      Accel_SetParameter(ACCEL_ENABLER, sizeof(valFalse), &valFalse);

      // Stop the acceleromter.
      // Note: Accel_SetParameter does not trigger the callback
      SensorTag_accelEnablerChangeCB();

      // Turn off LED that shows we're advertising.
#ifdef CC2640R2_LAUNCHXL
      PIN_setOutputValue(hGpioPin, Board_GLED, Board_LED_OFF);
#endif
#ifdef SENSORTAG_CC2640R2_LAUNCHXL
      PIN_setOutputValue(hGpioPin, BP_GLED, Board_LED_OFF);
#endif
      //SensorTag_resetAllModules();

      // Stop alert if it was active.
      if(sensortagAlertState != ALERT_STATE_OFF)
      {
        SensorTag_stopAlert();
      }
    }
    break;
  case GAPROLE_WAITING_AFTER_TIMEOUT:
    {
      // The link was dropped due to supervision timeout.
      sensortagProximityState = ST_PROXSTATE_LINK_LOSS;

      // Turn off immediate alert
      ProxReporter_SetParameter(PP_IM_ALERT_LEVEL, sizeof(valFalse), &valFalse);
      sensortagProxIMAlertLevel = PP_ALERT_LEVEL_NO;

      // Change attribute value of Accelerometer Enable to FALSE.
      Accel_SetParameter(ACCEL_ENABLER, sizeof(valFalse), &valFalse);

      // Stop the acceleromter.
      SensorTag_accelEnablerChangeCB(); // SetParameter does not trigger the callback

      // Perform link loss alert if enabled.
      if(sensortagProxLLAlertLevel != PP_ALERT_LEVEL_NO)
      {
        SensorTag_performAlert();
        buzzer_beep_count = 0;
      }

      SensorTag_resetAllModules();
    }
    break;

  case GAPROLE_ERROR:
    SensorTag_resetAllModules();
#ifdef CC2640R2_LAUNCHXL
    PIN_setOutputValue(hGpioPin, Board_RLED, Board_LED_ON);
#endif
#ifdef SENSORTAG_CC2640R2_LAUNCHXL
    PIN_setOutputValue(hGpioPin, BP_BLED, Board_LED_ON);
#endif
    break;

  default:
    break;
  }

  gapProfileState = newState;

}

/*******************************************************************************
 * @fn      SensorTag_charValueChangeCB
 *
 * @brief   Callback from Sensor Profile indicating a characteristic
 *          value change.
 *
 * @param   paramID - parameter ID of the value that was changed.
 *
 * @return  none
 */
void SensorTag_charValueChangeCB(uint8_t serviceID, uint8_t paramID)
{
  SensorTag_enqueueMsg(ST_CHAR_CHANGE_EVT, serviceID, paramID);
}

/*******************************************************************************
 * @fn      SensorTag_processCharValueChangeEvt
 *
 * @brief   Process pending Profile characteristic value change
 *          events. The events are generated by the network task (BLE)
 *
 * @param   serviceID - ID of the affected service
 * @param   paramID - ID of the affected parameter
 *
 * @return  none
 */
static void SensorTag_processCharValueChangeEvt(uint8_t serviceID,
                                                uint8_t paramID)
{
  switch (serviceID)
  {
  case SERVICE_ID_IO:
    SensorTagIO_processCharChangeEvt(paramID);
    break;

  case SERVICE_ID_BATT:
    SensorTagBatt_processCharChangeEvt(paramID);
    break;

  case SERVICE_ID_REGISTER:
    SensorTagRegister_processCharChangeEvt(paramID);
    break;

  case SERVICE_ID_CC:
    SensorTagConnControl_processCharChangeEvt(paramID);
    break;

  case SERVICE_ID_PROX:
    SensorTag_processProximityAttrEvt(paramID);
    break;

  default:
    break;
  }
}

/*******************************************************************************
 * @fn      SensorTag_processStackMsg
 *
 * @brief   Process an incoming stack message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static uint8_t SensorTag_processStackMsg(ICall_Hdr *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch (pMsg->event)
  {
    case GATT_MSG_EVENT:
      // Process GATT message
      safeToDealloc = SensorTag_processGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case HCI_GAP_EVENT_EVENT:
    {
        // Process HCI message
        switch(pMsg->status)
        {
          case HCI_COMMAND_COMPLETE_EVENT_CODE:
            // Process HCI Command Complete Event
            Log_info0("HCI Command Complete Event received");
            break;

          default:
            break;
        }
    }

    default:
      // do nothing
      break;
  }

  return (safeToDealloc);
}

/*******************************************************************************
 * @fn      SensorTag_processGATTMsg
 *
 * @brief   Process GATT messages
 *
 * @return  none
 */
static uint8_t SensorTag_processGATTMsg(gattMsgEvent_t *pMsg)
{

    // See if GATT server was unable to transmit an ATT response
    if (pMsg->hdr.status == blePending)
    {
      Log_warning1("Outgoing RF FIFO full. Re-schedule transmission of msg with opcode 0x%02x",
        pMsg->method);

      // No HCI buffer was available. Let's try to retransmit the response
      // on the next connection event.
      if (HCI_EXT_ConnEventNoticeCmd(pMsg->connHandle, selfEntityMain,
                                     ST_CONN_EVT_END_EVT) == SUCCESS)
      {
        // First free any pending response
        SensorTag_freeAttRsp(FAILURE);

        // Hold on to the response message for retransmission
        pAttRsp = pMsg;

        // Don't free the response message yet
        return (FALSE);
      }
    }
    else if (pMsg->method == ATT_FLOW_CTRL_VIOLATED_EVENT)
    {
      // ATT request-response or indication-confirmation flow control is
      // violated. All subsequent ATT requests or indications will be dropped.
      // The app is informed in case it wants to drop the connection.

      // Log the opcode of the message that caused the violation.
      Log_error1("Flow control violated. Opcode of offending ATT msg: 0x%02x",
        pMsg->msg.flowCtrlEvt.opcode);
    }
    else if (pMsg->method == ATT_MTU_UPDATED_EVENT)
    {
      // MTU size updated
      Log_info1("MTU Size change: %d bytes", pMsg->msg.mtuEvt.MTU);
    }
    else
    {
      // Got an expected GATT message from a peer.
      Log_info1("Recevied GATT Message. Opcode: 0x%02x", pMsg->method);
    }
    // Free message payload. Needed only for ATT Protocol messages
    GATT_bm_free(&pMsg->msg, pMsg->method);

    // It's safe to free the incoming message
    return (TRUE);
}

/*
 *  Application error handling functions
 *****************************************************************************/

/*
 * @brief   Send a pending ATT response message.
 *
 *          The message is one that the stack was trying to send based on a
 *          peer request, but the response couldn't be sent because the
 *          user application had filled the TX queue with other data.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_sendAttRsp(void)
{
  // See if there's a pending ATT Response to be transmitted
  if (pAttRsp != NULL)
  {
    uint8_t status;

    // Increment retransmission count
    rspTxRetry++;

    // Try to retransmit ATT response till either we're successful or
    // the ATT Client times out (after 30s) and drops the connection.
    status = GATT_SendRsp(pAttRsp->connHandle, pAttRsp->method, &(pAttRsp->msg));
    if ((status != blePending) && (status != MSG_BUFFER_NOT_AVAIL))
    {
      // Disable connection event end notice
      HCI_EXT_ConnEventNoticeCmd(pAttRsp->connHandle, selfEntityMain, 0);

      // We're done with the response message
      SensorTag_freeAttRsp(status);
    }
    else
    {
      // Continue retrying
      Log_warning2("Retrying message with opcode 0x%02x. Attempt %d",
        pAttRsp->method, rspTxRetry);
    }
  }
}


/*
 * @brief   Free ATT response message.
 *
 * @param   status - response transmit status
 *
 * @return  none
 */
static void SensorTag_freeAttRsp(uint8_t status)
{
  // See if there's a pending ATT response message
  if (pAttRsp != NULL)
  {
    // See if the response was sent out successfully
    if (status == SUCCESS)
    {
      Log_info2("Sent message with opcode 0x%02x. Attempt %d",
        pAttRsp->method, rspTxRetry);
    }
    else
    {
      Log_error2("Gave up message with opcode 0x%02x. Status: %d",
        pAttRsp->method, status);

      // Free response payload
      GATT_bm_free(&pAttRsp->msg, pAttRsp->method);
    }

    // Free response message
    ICall_freeMsg(pAttRsp);

    // Reset our globals
    pAttRsp = NULL;
    rspTxRetry = 0;
  }
}


/*******************************************************************************
 * @fn      SensorTag_performPeriodicTask
 *
 * @brief   Perform a periodic application task.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_performPeriodicTask(void)
{
  SensorTagRegister_update();
}

/*******************************************************************************
 * @fn      SensorTag_clockHandler
 *
 * @brief   Handler function for clock time-outs.
 *
 * @param   arg - event type
 *
 * @return  none
 */
static void SensorTag_clockHandler(UArg arg)
{
  Event_post(syncEvent, arg);
}

/*******************************************************************************
 * @fn      SensorTag_enqueueMsg
 *
 * @brief   Creates a message and puts the message in RTOS queue.
 *
 * @param   event - message event.
 * @param   serviceID - service identifier
 * @param   paramID - parameter identifier
 *
 * @return  none
 */
static void SensorTag_enqueueMsg(uint8_t event, uint8_t serviceID, uint8_t paramID)
{
  stEvt_t *pMsg;

  // Create dynamic pointer to message.
  if ((pMsg = ICall_malloc(sizeof(stEvt_t))))
  {
    pMsg->event = event;
    pMsg->serviceID = serviceID;
    pMsg->paramID = paramID;

    // Enqueue the message.
    Util_enqueueMsg(appMsgQueue, syncEvent, (uint8_t*)pMsg); //Add
  }
}


/*********************************************************************
 * @fn      SensorTag_resetAllModules
 *
 * @brief   Reset all modules, typically when a connection is terminated.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_resetAllModules(void)
{
  SensorTagIO_reset();
  SensorTagRegister_reset();
  SensorTagBatt_reset();
  SensorTagKeys_reset();
}

/*!*****************************************************************************
 *  @fn         SensorTag_callback
 *
 *  Interrupt service routine for buttons
 *
 *  @param      handle PIN_Handle connected to the callback
 *
 *  @param      pinId  PIN_Id of the DIO triggering the callback
 *
 *  @return     none
 ******************************************************************************/
static void SensorTag_callback(PIN_Handle handle, PIN_Id pinId)
{
  switch (pinId)
  {
  case Board_BTN1:
    SensorTagKeys_processKeyLeft();
    break;

  case Board_BTN2:
    SensorTagKeys_processKeyRight();
    break;

  default:
    /* Do nothing */
    break;
  }
}

/*******************************************************************************
 * @fn      SensorTag_updateAdvertisingData
 *
 * @brief   Update the advertising data with the latest key press status
 *
 * @return  none
 */
void SensorTag_updateAdvertisingData(uint8_t keyStatus)
{
  // Record key state in advertising data
  advertData[KEY_STATE_OFFSET] = keyStatus;
  GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
}

/*********************************************************************
 * @fn      SensorTag_processToggleBuzzerEvt
 *
 * @brief   Handles enabling and disabling of the buzzer and flashing LEDS
 *          during an alert.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SensorTag_processToggleBuzzerEvt(void)
{
  // If this event was triggered while buzzer is on, turn it off, increment
  // beep_count, check whether max has been reached, and if not set the TI-RTOS
  // timer for next event to turn buzzer back on.
  if (buzzer_state == BUZZER_ON)
  {
    SensorTagBuzzer_close();

    buzzer_state = BUZZER_OFF;
    buzzer_beep_count++;

    // Check to see if buzzer has beeped maximum number of times.
    // If it has, then disable clock.
    if ((buzzer_beep_count >= BUZZER_MAX_BEEPS)  ||
        ((sensortagProximityState != ST_PROXSTATE_LINK_LOSS) &&
         (sensortagProximityState != ST_PROXSTATE_PATH_LOSS)))
    {
      Util_stopClock(&toggleBuzzerClock);
    }
  }
  else if (sensortagAlertState != ALERT_STATE_OFF)
  {
    // If this event was triggered while the buzzer is off then turn it on if
    // appropriate.
    SensorTag_performAlert();
  }
}

/*********************************************************************
 * @fn      SensorTag_performAlert
 *
 * @brief   Perform alert.
 *
 * @param   None.
 *
 * @return  None.
 */
static void SensorTag_performAlert(void)
{

  SensorTagBuzzer_open(hGpioPin);

  if (sensortagProximityState == ST_PROXSTATE_LINK_LOSS)
  {
    switch(sensortagProxLLAlertLevel)
    {
      case PP_ALERT_LEVEL_LOW:
        sensortagAlertState = ALERT_STATE_LOW;

        SensorTagBuzzer_setFrequency(BUZZER_ALERT_LOW_FREQ);
        buzzer_state = BUZZER_ON;

        // Only run buzzer for 200ms.
        Util_startClock(&toggleBuzzerClock);

        // Turn off LEDs
        SensorTagIO_blinkLed(IOID_GREEN_LED, 1);
        break;

      case PP_ALERT_LEVEL_HIGH:
        sensortagAlertState = ALERT_STATE_HIGH;

        SensorTagBuzzer_setFrequency(BUZZER_ALERT_HIGH_FREQ);
        buzzer_state = BUZZER_ON;

        // Only run buzzer for 200ms.
        Util_startClock(&toggleBuzzerClock);

        //Turn on RED LED and blink the Green LED
        //PIN_setOutputValue(hGpioPin, BP_BLED, Board_LED_ON);
        SensorTagIO_blinkLed(IOID_GREEN_LED, 5);
        break;

      case PP_ALERT_LEVEL_NO:
          // Fall through
      default:
        SensorTag_stopAlert();
        break;
    }
  }
  else if (sensortagProximityState == ST_PROXSTATE_PATH_LOSS)
  {
    switch(sensortagProxIMAlertLevel)
    {
      case PP_ALERT_LEVEL_LOW:
        sensortagAlertState = ALERT_STATE_LOW;

        SensorTagBuzzer_setFrequency(BUZZER_ALERT_LOW_FREQ);
        buzzer_state = BUZZER_ON;

        // Only run buzzer for 200ms.
        Util_startClock(&toggleBuzzerClock);

        // Turn off LEDs
        SensorTagIO_blinkLed(IOID_GREEN_LED, 1);
        break;


      case PP_ALERT_LEVEL_HIGH:
        sensortagAlertState = ALERT_STATE_HIGH;

        SensorTagBuzzer_setFrequency(BUZZER_ALERT_HIGH_FREQ);
        buzzer_state = BUZZER_ON;

        // Only run buzzer for 200ms.
        Util_startClock(&toggleBuzzerClock);

        //Turn on RED LED and blink the Green LED
        //PIN_setOutputValue(hGpioPin, BP_BLED, Board_LED_ON);
        SensorTagIO_blinkLed(IOID_GREEN_LED, 5);
        break;

        case PP_ALERT_LEVEL_NO:
          // Fall through
        default:
          SensorTag_stopAlert();
          break;
    }
  }
}

/*********************************************************************
 * @fn      SensorTag_stopAlert
 *
 * @brief   Stops an alert.
 *
 * @param   none
 *
 * @return  none
 */
void SensorTag_stopAlert(void)
{

  Util_stopClock(&toggleBuzzerClock);

  sensortagAlertState = ALERT_STATE_OFF;

  SensorTagBuzzer_close();

  buzzer_state = BUZZER_OFF;

  // Turn LED's OFF
#ifdef CC2640R2_LAUNCHXL
      PIN_setOutputValue(hGpioPin, Board_RLED, Board_LED_OFF);
      PIN_setOutputValue(hGpioPin, Board_GLED, Board_LED_OFF);
#endif
#ifdef SENSORTAG_CC2640R2_LAUNCHXL
      PIN_setOutputValue(hGpioPin, BP_BLED, Board_LED_OFF);
      PIN_setOutputValue(hGpioPin, BP_GLED, Board_LED_OFF);
#endif
}

/*********************************************************************
 * @fn      SensorTag_proximityAttrCB
 *
 * @brief   Notification from the profile of an atrribute change by
 *          a connected device.
 *
 * @param   attrParamID - Profile's Attribute Parameter ID
 *            PP_LINK_LOSS_ALERT_LEVEL  - The link loss alert level value
 *            PP_IM_ALERT_LEVEL         - The immediate alert level value
 *
 * @return  none
 */
static void SensorTag_proximityAttrCB(uint8_t attrParamID)
{
  SensorTag_charValueChangeCB(SERVICE_ID_PROX, attrParamID);
}

/*********************************************************************
 * @fn      SensorTag_processProximityAttrEvt
 *
 * @brief   Process a notification from the profile of an atrribute
 *          change by a connected device.
 *
 * @param   attrParamID - Profile's Attribute Parameter ID
 *            PP_LINK_LOSS_ALERT_LEVEL  - The link loss alert level value
 *            PP_IM_ALERT_LEVEL         - The immediate alert level value
 *
 * @return  none
 */
static void SensorTag_processProximityAttrEvt(uint8_t attrParamID)
{
  switch(attrParamID)
  {
    case PP_LINK_LOSS_ALERT_LEVEL:
      ProxReporter_GetParameter(PP_LINK_LOSS_ALERT_LEVEL, &sensortagProxLLAlertLevel);
      break;

    case PP_IM_ALERT_LEVEL:
      ProxReporter_GetParameter(PP_IM_ALERT_LEVEL, &sensortagProxIMAlertLevel);

      // If proximity monitor set the immediate alert level to low or high, then
      // the monitor calculated that the path loss to the keyfob
      // (proximity observer) has exceeded the threshold.
      if(sensortagProxIMAlertLevel != PP_ALERT_LEVEL_NO)
      {
        sensortagProximityState = ST_PROXSTATE_PATH_LOSS;
        SensorTag_performAlert();
        buzzer_beep_count = 0;
      }
      // Proximity monitor turned off alert because the path loss is below
      // threshold.
      else
      {
        sensortagProximityState = ST_PROXSTATE_CONNECTED_IN_RANGE;
        SensorTag_stopAlert();
      }
      break;

    default:
      // should not reach here!
      break;
  }
}

/*********************************************************************
 * @fn      SensorTag_accelEnablerChangeCB
 *
 * @brief   Called by the Accelerometer Profile when the Enabler Attribute
 *          is changed.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_accelEnablerChangeCB(void)
{
  Event_post(syncEvent, ST_ACCEL_CHANGE_EVT);
}

/*********************************************************************
 * @fn      SensorTag_processAccelEnablerChangeEvt
 *
 * @brief   Process a callback from the Accelerometer Profile when the
 *          Enabler Attribute is changed.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_processAccelEnablerChangeEvt(void)
{
  if (Accel_GetParameter(ACCEL_ENABLER, &accelEnabler) == SUCCESS)
  {
    if (accelEnabler)
    {
      // Initialize accelerometer
      Acc_init();

      // Setup timer for accelerometer task
      Util_startClock(&accelReadClock);
    }
    else
    {
      // Stop the accelerometer
      Acc_stop();

      Util_stopClock(&accelReadClock);
    }
  }
}

/*********************************************************************
 * @fn      SensorTag_processAccelReadEvt
 *
 * @brief   Process accelerometer task.
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_processAccelReadEvt(void)
{
  // Check if accelerator is enabled.
  if (Accel_GetParameter(ACCEL_ENABLER, &accelEnabler) == SUCCESS)
  {
    if (accelEnabler)
    {
      // Restart timer
      if (ACCEL_READ_PERIOD)
      {
        Util_startClock(&accelReadClock);
      }

      // Read accelerometer data.
      SensorTag_accelRead();
    }
    else
    {
      // Stop the accelerometer.
      Util_stopClock(&accelReadClock);
    }
  }
}

/*********************************************************************
 * @fn      SensorTag_accelRead
 *
 * @brief   Called by the application to read accelerometer data
 *          and put data in accelerometer profile
 *
 * @param   none
 *
 * @return  none
 */
static void SensorTag_accelRead(void)
{
  static int8_t x = 0, y = 0, z = 0;
  int8_t new_x = 0, new_y = 0, new_z = 0;

  // Read data for each axis of the accelerometer
  Acc_readAcc(&new_x, &new_y, &new_z);

  // Check if x-axis value has changed by more than the threshold value and
  // set profile parameter if it has (this will send a notification if enabled)
  if((x < (new_x-ACCEL_CHANGE_THRESHOLD)) || (x > (new_x+ACCEL_CHANGE_THRESHOLD)))
  {
    x = new_x;
    Accel_SetParameter(ACCEL_X_ATTR, sizeof(int8_t), &x);
  }

  // Check if y-axis value has changed by more than the threshold value and
  // set profile parameter if it has (this will send a notification if enabled)
  if((y < (new_y-ACCEL_CHANGE_THRESHOLD)) || (y > (new_y+ACCEL_CHANGE_THRESHOLD)))
  {
    y = new_y;
    Accel_SetParameter(ACCEL_Y_ATTR, sizeof(int8_t), &y);
  }

  // Check if z-axis value has changed by more than the threshold value and
  // set profile parameter if it has (this will send a notification if enabled)
  if((z < (new_z-ACCEL_CHANGE_THRESHOLD)) || (z > (new_z+ACCEL_CHANGE_THRESHOLD)))
  {
    z = new_z;
    Accel_SetParameter(ACCEL_Z_ATTR, sizeof(int8_t), &z);
  }
}
/*******************************************************************************
*******************************************************************************/
