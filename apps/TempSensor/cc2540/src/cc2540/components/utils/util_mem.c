/*******************************************************************************
*  Filename:        util_mem.c
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Memory related utility functions
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
#include "util_mem.h"
#include "hal_types.h"


/*******************************************************************************
* LOCAL VARIABLES
*/


/*******************************************************************************
* GLOBAL FUNCTIONS
*/


/*******************************************************************************
* @fn       utilMemcpy
*
* @brief    Copy num bytes from pDest to pSrc
*
* @param    pDest - pointer to destination memory area
*           pSrc - pointer to source memory area
*           num - number of characters/bytes to copy
*
* @return   void
*/
void utilMemcpy(void __generic *pDest, const void __generic *pSrc, uint16 num) {
    uint8 __generic *pD = (uint8 __generic *) pDest;
    uint8 __generic *pS = (uint8 __generic *) pSrc;

    while (num--) {
        *(pD++) = *(pS++);
    }
}


/*******************************************************************************
* @fn       utilMemcmp
*
* @brief    Compares the first num bytes of two sequences s1 and s2.
*
* @param    pA  - pointer to memory area A
*           pB  - pointer to memory area B
*           num - number of characters/bytes to compare
*
* @return   0     - equal, pA == pB
*           other - not equal, pA != pB
*/
uint8 utilMemcmp(const void __generic *s1, const void __generic *s2, uint16 num) {
    uint8 __generic *a = (uint8 __generic *) s1;
    uint8 __generic *b = (uint8 __generic *) s2;

    for ( ; num--; a++, b++) {
        if (*a != *b) {
            return (*a - *b);
        }
    }
    return 0;
}