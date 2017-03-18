/******************************************************************************

 @file  cc26xx_stack.cmd

 @brief CC2650F128 linker configuration file for TI-RTOS with
        Code Composer Studio.

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

/* Retain interrupt vector table variable                                    */
--retain=g_pfnVectors
/* Override default entry point.                                             */
//--entry_point ResetISR
/* Suppress warnings and errors:                                             */
/* - 10063: Warning about entry point not being _c_int00                     */
/* - 16011, 16012: 8-byte alignment errors. Observed when linking in object  */ 
/*   files compiled using Keil (ARM compiler)                                */
--diag_suppress=10063,16011,16012

/* The following command line options are set as part of the CCS project.    */
    /* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
/* --heap_size=0                                                             */
/* --stack_size=256                                                          */
/* --library=rtsv7M3_T_le_eabi.lib                                           */

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application.                      */
#define FLASH_START       0x0
#define FLASH_SIZE        0x20000

#ifdef R2
#define RAM_ROM_RESERVE_SIZE 0x00000C00
#else /* R1 */
#define RAM_ROM_RESERVE_SIZE 0x00000718
#endif /* R2 */

#define RAM_START        0x20000000
#define RAM_END          0x00005000
#define RAM_SIZE         RAM_END - RAM_ROM_RESERVE_SIZE

#define WORD_SIZE        0x0004
#define PAGE_SIZE        0x1000

/* Configure the flash memory alignment of the start of this image */
#ifdef PAGE_ALIGN
#define FLASH_MEM_ALIGN      PAGE_SIZE
#else /* !PAGE_ALIGN */
#define FLASH_MEM_ALIGN      WORD_SIZE
#endif /* PAGE_ALIGN */

// Last page is reserved by Application for CCFG
#define NUM_RESERVED_PAGES      1
#define RESERVED_SIZE           (NUM_RESERVED_PAGES * PAGE_SIZE)

/* System memory map */
MEMORY
{
    /* EDITOR'S NOTE:
     * the FLASH and SRAM lengths can be changed by defining
     * ICALL_STACK0_START or ICALL_RAM0_START in
     * Properties->ARM Linker->Advanced Options->Command File Preprocessing.
     */

    /* Application stored in and executes from internal flash */
    //FLASH (RX) : origin = FLASH_START, length = FLASH_SIZE - RESERVED_SIZE -100
    FLASH (RX) : origin = FLASH_START, length = FLASH_SIZE - RESERVED_SIZE

    /* Application uses internal RAM for data */
    SRAM (RWX) : origin = RAM_START, length = RAM_SIZE
}

/* Section allocation in memory */
SECTIONS
{

	GROUP > FLASH(HIGH)  align FLASH_MEM_ALIGN
	{
		EntrySection
	    .text
	    .const
	    .constdata
	    .rodata
	    .cinit
	    .pinit
	    .init_array
	    .emb_text
	}

    .vtable         :   > SRAM(HIGH)
    .vtable_ram     :   > SRAM(HIGH)
     vtable_ram     :   > SRAM(HIGH)
    .data           :   > SRAM(HIGH)
    .bss            :   > SRAM(HIGH)
    .sysmem         :   > SRAM(HIGH)
    .nonretenvar    :   > SRAM(HIGH)
    .noinit         :   > SRAM(HIGH) TYPE=NOINIT

}

