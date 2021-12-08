/*******************************************************************************
*  Filename:        util_uart.c
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Utility library for UART.
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


/*******************************************************************************
* INCLUDES
*/

#include "hal_uart.h"
#include "util_uart.h"
#include "hal_defs.h"
#include "string.h"


/*******************************************************************************
* LOCAL FUNCTIONS
*/


/*******************************************************************************
* @fn      printChar
*
* @brief   Print a character on the console.
*
* @param   c - character to print
*
* @return  none
*/
static void printChar(char c) {
    halUartWrite((uint8*)&c, 1);
}


/*******************************************************************************
* @fn      printStrW
*
* @brief   Print a string padded with blanks.
*
* @param   sz - string to display
*          len - total length of the string
*
* @return  none
*/
static void printStrW(char *sz, uint8 nBytes) {
    printStr((char *)sz);
    if (strlen(sz)<nBytes) {
        nBytes-= strlen(sz);
        while(--nBytes>0)
            printChar(' ');
    }
}


/*******************************************************************************
* @fn      printHex
*
* @brief   Print an integer as hexadecimal.
*
* @param   v - the value to be displayed
*          s - size (8,16,32) | bit 7 to omit 'h' at the end
*
* @return  none
*/
static void printHex(uint32 v, uint8 s) {
    char buf[16];
    uint8 w;

    w= (s&~NO_HEX_INDICATION)>>2;        // String length

    if (s&NO_HEX_INDICATION) {
        buf[w]= '\0';
    } else {
        buf[w]= 'h';       // HEX
        buf[w+1]= '\0';    // String terminator
    }

    // Convert number
    do {
        w--;
        buf[w]= "0123456789ABCDEF"[v&0xF];
        v>>= 4;
    } while(w>0);

    // Print
    printStr(buf);
}


/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* @fn      printStr
*
* @brief   Print a zero-terminated string on the console
*
* @param   sz - string to send to the console
*
* @return  none
*/
void printStr(char* sz) {
    char lineFeed = '\r';
    halUartWrite((uint8*)sz, (uint16) strlen(sz));
    // if last character is newline, add carriage return
    if(*(sz+(strlen(sz)-1)) == '\n')
        halUartWrite((uint8*)&lineFeed,1);
}


/*******************************************************************************
* @fn      printKvpHex
*
* @brief   Print a Key-Value pair hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHex(char *szKey, uint16 v) {
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printHex(v,16);
    printChar('\n');
    printChar('\r');
}


/*******************************************************************************
* @fn      printKvpHex8bit
*
* @brief   Print a Key-Value pair hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHex8bit(char *szKey, uint8 v) {
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printHex(v,8);
    printChar('\n');
    printChar('\r');
}


/*******************************************************************************
* @fn      printKvpHexIEEE
*
* @brief   Print a Key-Value  hexadecimal
*
* @param   szKey - the value to be displayed
*          v - the value to be displayed
*
* @return  none
*/
void printKvpHexIEEE(char *szKey, uint8 *pIEEEAddress) {
    uint8 i = 8;
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    while (i > 0)
    {
        printHex(pIEEEAddress[--i], 8 | NO_HEX_INDICATION);
    }
    printStr("h\n\r");
}