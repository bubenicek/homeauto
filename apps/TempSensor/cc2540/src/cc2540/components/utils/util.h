/*******************************************************************************
*  Filename:        util.h
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Utility library header file
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


#ifndef UTIL_H
#define UTIL_H
/*******************************************************************************
* INCLUDES
*/
#include <hal_types.h>


/*******************************************************************************
* TYPEDEFS
*/
typedef struct {
  const char* szDescr;      // Textual description
  const uint8  value;        // Value
} menuItem_t;

typedef struct {
  const menuItem_t* pMenuItems;
  const uint8 nItems;
} menu_t;


/*******************************************************************************
* CONSTANTS AND DEFINES
*/

// Number of elements in an array
#define N_ITEMS(arr)                sizeof(arr)/sizeof(arr[0])


/*******************************************************************************
* GLOBAL FUNCTIONS
*/
char* convInt32ToText(int32 value);
const char* utilChipIdToStr(uint8 chipID);

#ifndef WIN32
uint8 min(uint8 v1, uint8 v2);
void utilReverseBuf(uint8* pBuf, uint8 length);
#endif

#endif