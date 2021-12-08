/*******************************************************************************
*  Filename:        util.c
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Utility library
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
#include "util.h"
#include "hal_defs.h"
#include "hal_rf.h"

/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* @fn          utilChipIdToStr
*
* @brief       Converts a chip ID to a text string.
*
* @param       uint8 chipID
*
* @return      none
*/
const char* utilChipIdToStr(uint8 chipID) {
    const char* szId;

    switch(chipID) {
    case HAL_RF_CHIP_ID_CC2420:
        szId= "2420";
        break;
    case HAL_RF_CHIP_ID_CC2430:
        szId= "2430";
        break;
    case HAL_RF_CHIP_ID_CC2431:
        szId= "2431";
        break;
    case HAL_RF_CHIP_ID_CC2520:
        szId= "2520";
        break;
    case HAL_RF_CHIP_ID_CC2530:
        szId= "2530";
        break;
    case HAL_RF_CHIP_ID_CC2531:
        szId= "2531";
        break;
    case HAL_RF_CHIP_ID_CC2533:
        szId= "2533";
        break;
    case HAL_RF_CHIP_ID_CC2510:
        szId= "2510";
        break;
    case HAL_RF_CHIP_ID_CC2511:
        szId= "2511";
        break;
    case HAL_RF_CHIP_ID_CC1110:
        szId= "1110";
        break;
    case HAL_RF_CHIP_ID_CC1111:
        szId= "1111";
        break;
    case HAL_RF_CHIP_ID_CC2541:
        szId= "2541";
        break;
    default:
        szId= "----";
    };

    return szId;
}


/*******************************************************************************
* @fn          convInt32ToText
*
* @brief       Converts 32 bit int to text
*
* @param       int32 value
*
* @return      char* - pointer to text buffer which is a file scope allocated array
*/
char* convInt32ToText(int32 value) {
    static char pValueToTextBuffer[12];
    char *pLast;
    char *pFirst;
    char last;
    uint8 negative;

    pLast = pValueToTextBuffer;

    // Record the sign of the value
    negative = (value < 0);
    value = ABS(value);

    // Print the value in the reverse order
    do {
        *(pLast++) = '0' + (uint8)(value % 10);
        value /= 10;
    } while (value);

    // Add the '-' when the number is negative, and terminate the string
    if (negative) *(pLast++) = '-';
    *(pLast--) = 0x00;

    // Now reverse the string
    pFirst = pValueToTextBuffer;
    while (pLast > pFirst) {
        last = *pLast;
        *(pLast--) = *pFirst;
        *(pFirst++) = last;
    }

    return pValueToTextBuffer;
}


#ifndef WIN32
/*******************************************************************************
* @fn          min
*
* @brief       Return minimum of two values
*
* @param       uint8 v1 - value 1
*              uint8 v2 - value 2
*
* @return      uint8 - minimum of two values
*/
uint8 min(uint8 v1, uint8 v2) {
    if(v1 < v2)
        return v1;
    else return v2;
}

/*******************************************************************************
* @fn          utilReverseBuf
*
* @brief       reverse buffer
*
* @param       uint8 pBuf - pointer to buffer
*              uint8 length - length of buffer
*
* @return      void
*/
void utilReverseBuf(uint8* pBuf, uint8 length) {
   uint8 temp;
   uint8* pBufLast = (pBuf + length - 1);

   while(pBufLast > pBuf){
     temp = *pBuf;
     *pBuf++ = *pBufLast;
     *pBufLast-- = temp;	
  }
}
#endif