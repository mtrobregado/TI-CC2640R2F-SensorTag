/******************************************************************************

 @file  osal.h

 @brief This API allows the software components in the Z-Stack to be written
        independently of the specifics of the operating system, kernel, or
        tasking environment (including control loops or connect-to-interrupt
        systems).

 Group: WCS, LPC, BTS
 Target Device: CC2640R2

 ******************************************************************************
 
 Copyright (c) 2004-2016, Texas Instruments Incorporated
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

#ifndef OSAL_H
#define OSAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include <limits.h>

#include "comdef.h"
#include "OSAL_Memory.h"
#include "OSAL_Timers.h"

#ifdef USE_ICALL
#include <icall.h>
#endif /* USE_ICALL */

/*********************************************************************
 * MACROS
 */
#if ( UINT_MAX == 65535 ) /* 8-bit and 16-bit devices */
  #define osal_offsetof(type, member) ((uint16) &(((type *) 0)->member))
#else /* 32-bit devices */
  #define osal_offsetof(type, member) ((uint32) &(((type *) 0)->member))
#endif

#define OSAL_MSG_NEXT(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->next

#define OSAL_MSG_Q_INIT(q_ptr)      *(q_ptr) = NULL

#define OSAL_MSG_Q_EMPTY(q_ptr)     (*(q_ptr) == NULL)

#define OSAL_MSG_Q_HEAD(q_ptr)      (*(q_ptr))

#define OSAL_MSG_LEN(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->len

#define OSAL_MSG_ID(msg_ptr)      ((osal_msg_hdr_t *) (msg_ptr) - 1)->dest_id

/*********************************************************************
 * CONSTANTS
 */

/*** Interrupts ***/
#define INTS_ALL    0xFF

/*********************************************************************
 * TYPEDEFS
 */
#ifdef USE_ICALL
typedef ICall_MsgHdr osal_msg_hdr_t;
#else /* USE_ICALL */
typedef struct
{
  void   *next;
#ifdef OSAL_PORT2TIRTOS
  /* Limited OSAL port to TI-RTOS requires compatibility with ROM
   * code compiled with USE_ICALL compile flag.  */
  uint32 reserved;
#endif /* OSAL_PORT2TIRTOS */
  uint16 len;
  uint8  dest_id;
} osal_msg_hdr_t;
#endif /* USE_ICALL */

typedef struct
{
  uint8  event;
  uint8  status;
} osal_event_hdr_t;

typedef void * osal_msg_q_t;

#ifdef USE_ICALL
/* High resolution timer callback function type */
typedef void (*osal_highres_timer_cback_t)(void *arg);
#endif /* USE_ICALL */

#ifdef ICALL_LITE
typedef void (*osal_icallMsg_hook_t)(void * param);
#endif /* ICALL_LITE */

/*********************************************************************
 * GLOBAL VARIABLES
 */
#ifdef USE_ICALL
#ifdef ICALL_EVENTS
extern ICall_SyncHandle osal_syncHandle;
#else /* !ICALL_EVENTS */
extern ICall_Semaphore osal_semaphore;
#endif /* ICALL_EVENTS */
extern ICall_EntityID osal_entity;
extern uint_least32_t osal_tickperiod;
extern void (*osal_eventloop_hook)(void);
#endif /* USE_ICALL */


/*********************************************************************
 * FUNCTIONS
 */

/*** Message Management ***/

  /*
   * Task Message Allocation
   */
  extern uint8 * osal_msg_allocate(uint16 len );

  /*
   * Task Message Deallocation
   */
  extern uint8 osal_msg_deallocate( uint8 *msg_ptr );

  /*
   * Send a Task Message
   */
  extern uint8 osal_msg_send( uint8 destination_task, uint8 *msg_ptr );

  /*
   * Push a Task Message to head of queue
   */
  extern uint8 osal_msg_push_front( uint8 destination_task, uint8 *msg_ptr );

  /*
   * Receive a Task Message
   */
  extern uint8 *osal_msg_receive( uint8 task_id );

  /*
   * Find in place a matching Task Message / Event.
   */
  extern osal_event_hdr_t *osal_msg_find(uint8 task_id, uint8 event);

  /*
   * Count the number of queued OSAL messages matching Task ID / Event.
   */
  extern uint8 osal_msg_count(uint8 task_id, uint8 event);

  /*
   * Enqueue a Task Message
   */
  extern void osal_msg_enqueue( osal_msg_q_t *q_ptr, void *msg_ptr );

  /*
   * Enqueue a Task Message Up to Max
   */
  extern uint8 osal_msg_enqueue_max( osal_msg_q_t *q_ptr, void *msg_ptr, uint8 max );

  /*
   * Dequeue a Task Message
   */
  extern void *osal_msg_dequeue( osal_msg_q_t *q_ptr );

  /*
   * Push a Task Message to head of queue
   */
  extern void osal_msg_push( osal_msg_q_t *q_ptr, void *msg_ptr );

  /*
   * Extract and remove a Task Message from queue
   */
  extern void osal_msg_extract( osal_msg_q_t *q_ptr, void *msg_ptr, void *prev_ptr );

#ifdef USE_ICALL
  extern ICall_Errno osal_service_entry(ICall_FuncArgsHdr *args);
#endif /* USE_ICALL */


/*** Task Synchronization  ***/

  /*
   * Set a Task Event
   */
  extern uint8 osal_set_event( uint8 task_id, uint16 event_flag );


  /*
   * Clear a Task Event
   */
  extern uint8 osal_clear_event( uint8 task_id, uint16 event_flag );


/*** Interrupt Management  ***/

  /*
   * Register Interrupt Service Routine (ISR)
   */
  extern uint8 osal_isr_register( uint8 interrupt_id, void (*isr_ptr)( uint8* ) );

  /*
   * Enable Interrupt
   */
  extern uint8 osal_int_enable( uint8 interrupt_id );

  /*
   * Disable Interrupt
   */
  extern uint8 osal_int_disable( uint8 interrupt_id );


/*** Task Management  ***/

#ifdef USE_ICALL
  /*
   * Enroll dispatcher registered entity ID
   */
  extern void osal_enroll_dispatchid(uint8 taskid,
                                     ICall_EntityID dispatchid);

  /*
   * Enroll an OSAL task to use another OSAL task's enrolled entity ID
   * when sending a message.
   */
  extern void osal_enroll_senderid(uint8 taskid, ICall_EntityID dispatchid);

  /*
   * Enroll entity ID to be used as sender entity ID for non OSAL task
   */
  extern void osal_enroll_notasksender(ICall_EntityID dispatchid);
  
#ifdef ICALL_JT  
  /* 
  * Initialize osal timer module variable at init, 
  * based on parameter send by the application.
  */
  void osal_timer_init(uint_least32_t tickPeriod, uint_least32_t osalMaxMsecs);
#endif /* ICALL_JT */
    
#endif /* USE_ICALL */

  /*
   * Initialize the Task System
   */
  extern uint8 osal_init_system( void );

  /*
   * System Processing Loop
   */
#if defined (ZBIT)
  extern __declspec(dllexport)  void osal_start_system( void );
#else
  extern void osal_start_system( void );
#endif

  /*
   * One Pass Through the OSAL Processing Loop
   */
  extern void osal_run_system( void );

  /*
   * Get the active task ID
   */
  extern uint8 osal_self( void );


/*** Helper Functions ***/

  /*
   * String Length
   */
  extern int osal_strlen( char *pString );

  /*
   * Memory copy
   */
  extern void *osal_memcpy( void*, const void GENERIC *, unsigned int );

  /*
   * Memory Duplicate - allocates and copies
   */
  extern void *osal_memdup( const void GENERIC *src, unsigned int len );

  /*
   * Reverse Memory copy
   */
  extern void *osal_revmemcpy( void*, const void GENERIC *, unsigned int );

  /*
   * Memory compare
   */
  extern uint8 osal_memcmp( const void GENERIC *src1, const void GENERIC *src2, unsigned int len );

  /*
   * Memory set
   */
  extern void *osal_memset( void *dest, uint8 value, int len );

  /*
   * Build a uint16 out of 2 bytes (0 then 1).
   */
  extern uint16 osal_build_uint16( uint8 *swapped );

  /*
   * Build a uint32 out of sequential bytes.
   */
  extern uint32 osal_build_uint32( uint8 *swapped, uint8 len );

  /*
   * Convert long to ascii string
   */
  #if !defined ( ZBIT ) && !defined ( ZBIT2 ) && !defined (UBIT)
    extern uint8 *_ltoa( uint32 l, uint8 * buf, uint8 radix );
  #endif

  /*
   * Random number generator
   */
  extern uint16 osal_rand( void );

  /*
   * Buffer an uint32 value - LSB first.
   */
  extern uint8* osal_buffer_uint32( uint8 *buf, uint32 val );

  /*
   * Buffer an uint24 value - LSB first
   */
  extern uint8* osal_buffer_uint24( uint8 *buf, uint24 val );

  /*
   * Is all of the array elements set to a value?
   */
  extern uint8 osal_isbufset( uint8 *buf, uint8 val, uint8 len );

#ifdef ICALL_LITE
  /*
   * set the callback to parse icall lite message.
   */
  extern void osal_set_icall_hook( osal_icallMsg_hook_t param );
  
  /*
   * translate alien task Id to osal task id.
   */
  extern uint8 osal_alien2proxy(ICall_EntityID entity);

#endif /* ICALL_LITE */  

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OSAL_H */
