/*******************************************************************************
*  Filename:        util_ring_buf.h
*  Revised:         $Date: 2014-01-10 11:57:48 +0100 (fr, 10 jan 2014) $
*  Revision:        $Revision: 11713 $
*
*  Description:     Header file for ring buffer interface. The ring buffer
*                   interface uses DMA channels 1, 2 and 3.
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
*******************************************************************************/


#ifndef UTIL_RING_BUF_H
#define UTIL_RING_BUF_H
/*******************************************************************************
* INCLUDES
*/
#include <hal_types.h>
#include <dma.h>


/*******************************************************************************
* TYPEDEFS
*/
/// Generic ringbuffer implementation control structure
__idata typedef struct {
    __xdata uint8 *start;               ///< Lowest address in buffer
    __xdata uint8 *end;                 ///< Highest address in buffer + 1
    __xdata uint8 *head;                ///< Buffer data input pointer (push)
    __xdata uint8 *tail;                ///< Buffer data output pointer (pop)
    uint16 numStoredBytes;              ///< Number of stored bytes
} RingBufCtrl_t;


/*******************************************************************************
* MACROS
*/
// Defined DMA channels
#define DMA_STATIC_DST_CHANNEL      1   ///< Ch. for static destination transfers.
#define DMA_STATIC_SRC_CHANNEL      2   ///< Ch. for transfers with static source addr.
#define DMA_MEMCOPY_CHANNEL         3   ///< Ch. for with src/dst addr. incrementing.


/*******************************************************************************
* VARIABLES
*/
extern DMA_DESC __xdata pDmaDesc[5];


/*******************************************************************************
* FUNCTIONS
*/
void    ringBufInit(RingBufCtrl_t *pRbc, XDATA uint8 *pBuffer, uint16 bufSize);
uint16  ringBufGetFreeSpace(RingBufCtrl_t *pRbc);
uint16  ringBufGetNumBytes(RingBufCtrl_t *pRbc);
void    ringBufPushByte(RingBufCtrl_t *pRbc, uint8 byte);
uint16  ringBufPush(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes);
uint16  ringBufPushStaticSrc(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes);
void    ringBufPopByte(RingBufCtrl_t *pRbc, uint8 *pDst);
uint16  ringBufPop(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes);
uint16  ringBufPopStaticDst(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes);
uint16  ringBufPeek(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes);
void    ringBufFlush(RingBufCtrl_t *pRbc);

// Functions using DMA
void    ringBufDmaInit(void);
uint16  ringBufPushDma(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes);
uint16  ringBufPushStaticSrcDma(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes);
uint16  ringBufPopStaticDstDma(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes);


#endif // #ifndef UTIL_RING_BUF_H