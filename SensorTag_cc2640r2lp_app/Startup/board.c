/*******************************************************************************

 @file  board.c

 @brief This file is a simple gateway to include the appropriate board.h file
        which is located in a corresponding subdirectory relative to this file.
        In this way, all projects look the same and only need to include this
        board.h. To change a board, the project only need update the board
        define. Presently, the following board targets are possible:

        CC2640R2DK_CXS
		CC2640R2_LAUNCHXL

        If a project needs to change the board defined for a particular target,
        they can modify the board.h located in the corresponding board
        subdirectory.

        NOTE: THIS FILE SHOULD ONLY BE CHANGED TO ADD A NEW BOARD/TARGET!

 Group: WCS, LPC, BTS
 Target Device: CC2640R2

 *******************************************************************************
 
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

 *******************************************************************************
 Release Name: ti-ble-3.0-stack-sdk_3_00_00
 Release Date: 2016-12-21 12:44:47
 ******************************************************************************/

#if defined(CC2640R2DK_CXS)
    #include "./cc2640r2em/cc2640r2em_board.c"
#elif defined(CC2640R2_LAUNCHXL)
    #include "./cc2640r2lp/cc2640r2lp_board.c"
#elif defined(SENSORTAG_CC2640R2_LAUNCHXL)
    #include "../../boards/SENSORTAG_CC2640R2_LAUNCHXL/Board.h"
    #include "../../boards/SENSORTAG_CC2640R2_LAUNCHXL/SENSORTAG_CC2640R2_LAUNCHXL.c"
#else // unknown board
    #error "***ERROR*** Invalid Board Specified! Please see board.h for options."
#endif
