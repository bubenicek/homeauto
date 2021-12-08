/*******************************************************************************
*  Filename:        util_console.c
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


#ifdef WIN32
/*******************************************************************************
 * INCLUDES
 */
#include "hal_console.h"
#include "util_console.h"
#include "string.h"


/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * Global data
 */

static uint8 fdConsole= FD_UART;

/*******************************************************************************
* FUNCTIONS
*/

/*******************************************************************************
* @fn      setStdout
* @brief   Redirect the standard output stream
* @param   fd - console file descriptor (UART or remote).
* @return  none
*/
void setStdout(uint8 fd) {
    fdConsole= fd;
}


/*******************************************************************************
* @fn      printChar
* @brief   Print a character on the console.
* @param   c - character to print
* @return  none
*/
void printChar(char c) {
    if (fdConsole==FD_UART) {
        halConsolePrintChar(c);
    } else {
        // TBD
    }
}


/*******************************************************************************
* @fn      printStr
* @brief   Print a zero-terminated string on the console
* @param   sz - string to send to the console
* @return  none
*/
void printStr(char FAR *sz) {

    if (fdConsole==FD_UART) {
       halConsolePrintStr(sz);
    } else {
        // TBD
    }
}


/*******************************************************************************
 * @fn      printStrCode
 * @brief   Print a string store in code memory.
 * @param   none
 * @return  none
 */
void printStrCode(const char CODE *p) {
    while (*p!='\0') {
        printChar(*p++);
    }
}


/*******************************************************************************
* @fn      printStrW
* @brief   Print a string padded with blanks.
* @param   sz - string to display
* @param   len - total length of the string
* @return  none
*/
void printStrW(char FAR *sz, uint8 nBytes) {
    printStr((char FAR *)sz);
    if (strlen(sz)<nBytes) {
        nBytes-= strlen(sz);
        while(--nBytes>0)
            printChar(' ');
    }

}


/*******************************************************************************
* @fn      printLine
* @brief   Print a delimiter line.
* @param   none
* @return  none
*/
void printLine(uint8 c, uint8 n) {
    while(n-->0)
        printChar(c);
    printChar('\n');
}

/*******************************************************************************
* @fn      printKvpHex
* @brief   Print a Key-Value pair hexadecimal
* @param   szKey - the value to be displayed
* @param   v - the value to be displayed
* @return  none
*/
void printKvpHex(char FAR *szKey, uint16 v) {
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printHex(v,16);
    printChar('\n');
}


/*******************************************************************************
* @fn      printKvpInt
* @brief   Print a Key-Value pair hexadeciaml.
* @param   szKey - the value to be displayed
* @param   v - the value to be displayed
* @return  none
*/
void printKvpInt(char FAR *szKey, uint32 v) {
    printStrW(szKey, KVP_ALIGN);
    printStr(": ");
    printInt(v);
    printChar('\n');
}


/*******************************************************************************
* @fn      printTime
* @brief   Print time in HH:MM:SS:ms format
* @param   nMsec - milliseconds
* @return  none
*/
void printTime(uint32 nMsec) {
    uint32 nSec;
    uint16 nMin;
    uint16 nHour;

    nSec= nMsec / 1000;
    nMin= (uint16)nSec / 60;
    nHour= nMin / 60;

    printStr("Time: ");
    printInt(nHour);
    printStr(":");

    nMin= nMin % 60;
    if (nMin<10)
        printStr("0");
    printInt(nMin);
    printStr(":");

    nSec= nSec % 60;
    if (nSec<10)
        printStr("0");
    printInt(nSec);
    printStr(".");

    nMsec= nMsec % 1000;
    if (nMsec<100) {
        printStr("0");
        if (nMsec<10) {
            printStr("0");
        }
    }
    printInt(nMsec);
    printChar('\n');
}


/*******************************************************************************
* @fn      printInt
* @brief   Print a 32-bit integer as decimal.
* @param   v - the value to be displayed
* @return  none
*/
void printInt(uint32 v) {
    uint32 iDiv;
    uint8 bLeadingZero;

    // Initialise
    iDiv= 1000000000;
    bLeadingZero= 1;

    // For each digit
    while (iDiv>0) {
        uint8 vDigit;

        // Current digit
        vDigit= (uint8)(v / iDiv);

        // First digit to be printed
        if (vDigit>0) {	
            bLeadingZero= 0;
        }

        if (!bLeadingZero) {
            printChar( (char)('0' + vDigit) );
        }

        // Next digit
        v= v % iDiv;
        iDiv= iDiv/10;
    }

    // Special case zero
    if (bLeadingZero) {
        printChar('0');
    }
}


/*******************************************************************************
* @fn      printHex
* @brief   Print an integer as hexadeciaml.
* @param   v - the value to be displayed
* @param   s - size (8,16,32) | bit 7 to omit 'h' at the end
* @return  none
*/
void printHex(uint32 v, uint8 s) {
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

#endif