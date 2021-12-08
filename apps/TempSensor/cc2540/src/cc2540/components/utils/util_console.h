/*******************************************************************************
*  Filename:        util_console.h
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Various console functions.
*
*
*  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/


#ifndef UTIL_CONSOLE_H
#define UTIL_CONSOLE_H


#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"


/*******************************************************************************
 * COMPILER switches
 */

// For use with AVR (RAM shortage)
#if defined (CC2420DB)
#define CONSOLE_REDUCED_OUTPUT
#endif

// For use with CC2430, non-banked mode (flash shortage)
#ifdef __ICC8051__
#if __CODE_MODEL__==2
#define CONSOLE_REDUCED_OUTPUT
#endif
#endif

  
/*******************************************************************************
 * CONSTANTS
 */

#define NO_HEX_INDICATION   0x80    // display hex numbers without trailing 'h'
#define KVP_ALIGN           24      // Default padding for printKvpInt/hex


// File descriptor for the operator console
#define FD_UART             0
#define FD_REMOTE           1

  
/*******************************************************************************
 * MACROS
 */

// Print the name and value of a structure member reference (HEX)
#define printKvpHexM(p,m)   printKvpHex(#m##, p->m)

// Print the name and value of a structure member reference (INT)
#define printKvpIntM(p,m)   printKvpInt(#m##, p->m)

// Print the name and value of a variable (INT)
#define printKvpIntV(v)     printKvpInt(#v##, v)

// Print the name and value of a variable (HEX)
#define printKvpHexV(v)     printKvpHex(#v##, v)

  
/*******************************************************************************
 * TYPEDEFS
 */

  
/*******************************************************************************
 * PUBLIC FUNCTIONS
 */

void    printChar(char c);
void    printStr(char FAR *sz);
void    printInt(uint32 i);
void    printHex(uint32 qw, uint8 iRadix);
void    printKvpInt(char FAR *szKey, uint32 v);
void    printKvpHex(char FAR *szKey, uint16 v);

void    setStdout(uint8 fd);
void    printTime(uint32 nMsec);
void    printStrW(char FAR *sz, uint8 len);
void    printMenu(void);
void    printLine(uint8 c, uint8 n);
void    printStrCode(const char CODE *p);


#ifdef __cplusplus
}
#endif

#endif