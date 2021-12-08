/*******************************************************************************
*  Filename:        util_buffer.h
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Ringbuffer interface
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


#ifndef UTIL_BUFFER_H
#define UTIL_BUFFER_H
/*******************************************************************************
* INCLUDES
*/
#include <hal_types.h>


/*******************************************************************************
* CONSTANTS AND DEFINES
*/
#ifndef BUF_SIZE
#ifdef WIN32
#define BUF_SIZE	256
#else
#define BUF_SIZE    128
#endif
#endif

#if (BUF_SIZE > 256)
#error "Buffer size too big"
#endif

/*******************************************************************************
* TYPEDEFS
*/
typedef struct {
    volatile uint8 pData[BUF_SIZE];
    volatile uint8 nBytes;
    volatile uint8 iHead;
    volatile uint8 iTail;
} ringBuf_t;

/*******************************************************************************
* MACROS
*/
#define bufFlush(pBuf)  bufInit(pBuf)

/*******************************************************************************
* GLOBAL FUNCTIONS
*/
void  bufInit(ringBuf_t *pBuf);
uint8 bufPut(ringBuf_t *pBuf, const uint8 *pData, uint8 n);
uint8 bufGet(ringBuf_t *pBuf, uint8 *pData, uint8 n);
uint8 bufPeek(ringBuf_t *pBuf, uint8 *pData, uint8 nBytes);
uint8 bufNumBytes(ringBuf_t *pBuf);

#endif