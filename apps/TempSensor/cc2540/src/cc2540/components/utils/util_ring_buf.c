/*******************************************************************************
*  Filename:        util_ring_buf.c
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Ring buffer (aka circular buffer) implementation.
*                   The implementation can support DMA transfers and is
*                   optimized for the USB UART handler.
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
/// @addtogroup util_ring_buf
/// @{ @ingroup uart_handler usb_uart_handler

/******************************************************************************
* INCLUDES
*/

#include <hal_board.h>
#include <hal_int.h>
#include <hal_defs.h>
#include <hal_assert.h>
#include "util_ring_buf.h"


/******************************************************************************
* MACROS
*/

#define GET_BUF_SIZE(pRbc)          ((uint16)((pRbc)->end) - (uint16)((pRbc)->start))
#define GET_USED_SPACE(pRbc)        ((pRbc)->numStoredBytes)
#define GET_FREE_SPACE(pRbc)        (GET_BUF_SIZE(pRbc) - ((pRbc)->numStoredBytes))

#define IS_BUF_EMPTY(pRbc)          ((pRbc)->numStoredBytes == 0)
#define IS_BUF_FULL(pRbc)           (GET_USED_SPACE(pRbc) == GET_BUF_SIZE(pRbc))

/******************************************************************************
* VARIABLES
*/


/******************************************************************************
* FUNCTIONS
*/

/**************************************************************************//**
* @fn       ringBufInit(RingBufCtrl_t * pRbc, uint8 __xdata *pBuffer, uint16 bufSize)
*
* @brief    Initialize a ring buffer. The memory to be used must be allocated
*           by the application in advance.
*           @note One byte will always remain unused in order to distinguish
*                 between empty/full buffer.
*
* @param    pRbc        Pointer to an uninitialized ring buffer controller
* @param    pBuffer     Pointer to the ring buffer start address
* @param    bufSize     Size of buffer, in bytes
*
* @return   none
******************************************************************************/
void ringBufInit(RingBufCtrl_t * pRbc, uint8 __xdata *pBuffer, uint16 bufSize)
{
    pRbc->start = pBuffer;
    pRbc->end = pBuffer + bufSize;
    pRbc->head = pBuffer;
    pRbc->tail = pBuffer;
    pRbc->numStoredBytes = 0;
} // ringBufInit


/**************************************************************************//**
* @fn       ringBufDmaInit(void)
*
* @brief    Setup DMA configuration table used by the ringbuffer implementation.
*           DMA ch. 1: XDATA/CODE to a static destination
*           DMA ch. 2: Static source to XDATA/CODE
*           DMA ch. 3: XDATA/CODE to XDATA/CODE
*
* @return   none
******************************************************************************/
void ringBufDmaInit(void)
{
    // Abort any ongoing DMA transfers
    DMA_CHANNEL_ABORT( BM(DMA_STATIC_DST_CHANNEL) | \
                       BM(DMA_STATIC_SRC_CHANNEL) | \
                       BM(DMA_MEMCOPY_CHANNEL) );

    // Bind the DMA controller to the configuration table
    DMA_SET_ADDR_DESC1234( (uint16) &pDmaDesc[1] );

    // Channel 1:   XDATA/CODE -> Static destination (USB EP IN FIFO)
    pDmaDesc[DMA_STATIC_DST_CHANNEL].lenH       = 0x00;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].vlen       = DMA_VLEN_USE_LEN;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].priority   = DMA_PRI_LOW;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].m8         = DMA_M8_USE_8_BITS;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].irqMask    = DMA_IRQMASK_DISABLE;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].srcInc     = DMA_SRCINC_1;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].destInc    = DMA_DESTINC_0;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].trig       = DMA_TRIG_NONE;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].tMode      = DMA_TMODE_BLOCK;
    pDmaDesc[DMA_STATIC_DST_CHANNEL].wordSize   = DMA_WORDSIZE_BYTE;

    // Channel 2:   Static source (USB EP OUT FIFO) -> XDATA/CODE
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].lenH       = 0x00;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].vlen       = DMA_VLEN_USE_LEN;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].priority   = DMA_PRI_LOW;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].m8         = DMA_M8_USE_8_BITS;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].irqMask    = DMA_IRQMASK_DISABLE;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].srcInc     = DMA_SRCINC_0;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].destInc    = DMA_DESTINC_1;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].trig       = DMA_TRIG_NONE;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].tMode      = DMA_TMODE_BLOCK;
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].wordSize   = DMA_WORDSIZE_BYTE;

    // Channel 3:   XDATA/CODE -> XDATA
    pDmaDesc[DMA_MEMCOPY_CHANNEL].lenH          = 0x00;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].vlen          = DMA_VLEN_USE_LEN;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].priority      = DMA_PRI_LOW;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].m8            = DMA_M8_USE_8_BITS;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].irqMask       = DMA_IRQMASK_DISABLE;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].srcInc        = DMA_SRCINC_1;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].destInc       = DMA_DESTINC_1;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].trig          = DMA_TRIG_NONE;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].tMode         = DMA_TMODE_BLOCK;
    pDmaDesc[DMA_MEMCOPY_CHANNEL].wordSize      = DMA_WORDSIZE_BYTE;

    // Channel 4:   Not used, but disabling IRQMASK and DMA trigging.
//    pDmaDesc[4].irqMask     = DMA_IRQMASK_DISABLE;
//    pDmaDesc[4].trig        = DMA_TRIG_NONE;

} // ringBufDmaInit


/**************************************************************************//**
* @fn      ringBufGetFreeSpace(RingBufCtrl_t *pRbc)
*
* @brief   Get the amount of free space in buffer. Since one byte is always
*          left unused, the ringBufGetFreeSpace() free space will always be at
*          least one byte less than the allocated memory.
*
* @param   pRbc Pointer to the ringbuffer controller
*
* @return  Number of bytes available in buffer (uint16).
******************************************************************************/
uint16 ringBufGetFreeSpace(RingBufCtrl_t *pRbc)
{
    return GET_FREE_SPACE(pRbc);
} // ringBufGetFreeSpace


/**************************************************************************//**
* @fn      ringBufGetNumBytes(RingBufCtrl_t *pRbc)
*
* @brief   Get the number of bytes stored in ringbuffer.
*
* @param   pRbc Pointer to the ring buffer controller
*
* @return  Number of bytes in buffer
******************************************************************************/
uint16 ringBufGetNumBytes(RingBufCtrl_t *pRbc)
{
    return GET_USED_SPACE(pRbc);
} // ringBufGetNumBytes


/**************************************************************************//**
* @fn      ringBufPushByte(RingBufCtrl_t *pRbc, uint8 byte)
*
* @brief   Push single byte to the buffer.
*          @warning This function does not check for buffer overflow!
*
* @param   pRbc Pointer to the ring buffer controller
* @param   byte Byte to write
*
* @return  none
******************************************************************************/
void ringBufPushByte(RingBufCtrl_t *pRbc, uint8 byte)
{
    uint16 s;

    // Critical section start
    s = halIntLock();

    *pRbc->head++ = byte;   // Put byte to address pointed to by head, then increment head
    pRbc->numStoredBytes++; // Increment numStoredBytes

    // Manage ring buffer wrap-around
    if (pRbc->head == pRbc->end) {
        pRbc->head = pRbc->start;
    }
    // Critical section end
    halIntUnlock(s);
} // ringBufPushByte


/**************************************************************************//**
* @fn      ringBufPush(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
*
* @brief   Push bytes to the buffer. Uses CPU (not DMA) for copying.
*
* @param   pRbc     Pointer to the ring buffer controller
* @param   pSrc     Pointer to source array with data to push
* @param   numBytes Number of bytes to push
*
* @return  Actual number of bytes pushed to buffer
******************************************************************************/
uint16 ringBufPush(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
{
    uint16 s, i, n;

    // Critical section start
    s = halIntLock();

    n = MIN(numBytes, GET_FREE_SPACE(pRbc));

    for (i = 0; i < n; i++) {
        *pRbc->head++ = pSrc[i];

        if (pRbc->head == pRbc->end) {
            pRbc->head = pRbc->start;
        }
    }
    pRbc->numStoredBytes += i;

    // Critical section end
    halIntUnlock(s);

    return i;
} // ringBufPush


/**************************************************************************//**
* @fn      ringBufPushDma(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
*
* @brief   Push bytes from XDATA to the buffer. Uses DMA for copying.
*          Function assumes that no other function (e.g. ISR) pushes data to
*          the ringbuffer while it is running. ISR based poping is ok.
*
* @param   pRbc     Pointer to the ring buffer controller
* @param   pSrc     Pointer to source address with data to push
* @param   numBytes Number of bytes to push
*
* @return  Actual number of bytes pushed to buffer
******************************************************************************/
uint16 ringBufPushDma(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
{
    uint16 s, n, firstChunk, lastChunk;

    // How many bytes can we transfer before buffer is full?
    s = halIntLock();   // Critical section start (reading 16 bit variable pRbc->numStoredBytes that is altered by an ISR)
    n = MIN(numBytes, GET_FREE_SPACE(pRbc));
    halIntUnlock(s);    // Critical section end

    // Do we need two DMA transfers?
    firstChunk = MIN(n, (pRbc->end - pRbc->head));
    lastChunk = n - firstChunk;

    // 1. First chunk
    // Update DMA configuration table
    pDmaDesc[DMA_MEMCOPY_CHANNEL].srcAddrH  = HI_UINT16(pSrc);
    pDmaDesc[DMA_MEMCOPY_CHANNEL].srcAddrL  = LO_UINT16(pSrc);
    pDmaDesc[DMA_MEMCOPY_CHANNEL].destAddrH = HI_UINT16(pRbc->head);
    pDmaDesc[DMA_MEMCOPY_CHANNEL].destAddrL = LO_UINT16(pRbc->head);
    pDmaDesc[DMA_MEMCOPY_CHANNEL].lenH = HI_UINT16(firstChunk);
    pDmaDesc[DMA_MEMCOPY_CHANNEL].lenL = LO_UINT16(firstChunk);

    // Wait if channel is currently active
    while (DMA_CHANNEL_ACTIVE( BM(DMA_MEMCOPY_CHANNEL) ));

    // Load DMA configuration and start the DMA channel
    DMA_CHANNEL_ARM( BM(DMA_MEMCOPY_CHANNEL) );
    DMA_CHANNEL_TRIG( BM(DMA_MEMCOPY_CHANNEL) );

    if(lastChunk > 0)
    {
        // 2. Second chunk
        // Wait for the first chunk to complete
        while (DMA_CHANNEL_ACTIVE( BM(DMA_MEMCOPY_CHANNEL) ));

        // Update DMA configuration table
        pDmaDesc[DMA_MEMCOPY_CHANNEL].srcAddrH  = HI_UINT16(pSrc + firstChunk);
        pDmaDesc[DMA_MEMCOPY_CHANNEL].srcAddrL  = LO_UINT16(pSrc + firstChunk);
        pDmaDesc[DMA_MEMCOPY_CHANNEL].destAddrH = HI_UINT16(pRbc->start);
        pDmaDesc[DMA_MEMCOPY_CHANNEL].destAddrL = LO_UINT16(pRbc->start);
        pDmaDesc[DMA_MEMCOPY_CHANNEL].lenH = HI_UINT16(lastChunk);
        pDmaDesc[DMA_MEMCOPY_CHANNEL].lenL = LO_UINT16(lastChunk);

        // Load DMA configuration and start the DMA channel
        DMA_CHANNEL_ARM( BM(DMA_MEMCOPY_CHANNEL) );
        DMA_CHANNEL_TRIG( BM(DMA_MEMCOPY_CHANNEL) );

        // Update ringbuffer's head value
        pRbc->head = pRbc->start + lastChunk;
    }
    else
    {
        // Only a single DMA transfer was necessary:
        // Update ringbuffer's head value
        pRbc->head += firstChunk;
        // Handle wrap-around
        if (pRbc->head == pRbc->end)
        {
            pRbc->head = pRbc->start;
        }
    }

    // Update number of stored bytes
    s = halIntLock(); // Critical section start (read/modify/write of 16 bit variable that is altered by an ISR)
    pRbc->numStoredBytes += n;
    halIntUnlock(s); // Critical section end

    return n;
} // ringBufPushDma



/**************************************************************************//**
* @fn      ringBufPushStaticSrc(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
*
* @brief   Push bytes from a static address to the buffer, i.e. without
*          incrementing source address. Uses CPU (not DMA) for copying.
*
* @param   pRbc     Pointer to the ring buffer controller
* @param   pSrc     Pointer to source address with data to push
* @param   numBytes Number of bytes to push
*
* @return  Actual number of bytes pushed to buffer
******************************************************************************/
uint16 ringBufPushStaticSrc(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
{
    uint16 s, i, n;

    // Critical section start
    s = halIntLock();
    n = MIN(numBytes, GET_FREE_SPACE(pRbc));

    for (i = 0; i < n; i++) {
        *pRbc->head++ = *pSrc;

        if (pRbc->head == pRbc->end) {
            pRbc->head = pRbc->start;
        }
    }
    pRbc->numStoredBytes += i;
    // Critical section end
    halIntUnlock(s);

    return i;
} // ringBufPushStaticSrc


/**************************************************************************//**
* @fn      ringBufPushStaticSrcDma(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
*
* @brief   Push bytes from a static address to the buffer. Uses DMA for copying.
*          Function assumes that no other function (e.g. ISR) pushes data to
*          the ringbuffer while it is running. ISR based poping is ok.
*          @warning For efficiency, only transfers <256B are supported!
*
* @param   pRbc     Pointer to the ring buffer controller
* @param   pSrc     Pointer to source address with data to push
* @param   numBytes Number of bytes to push
*
* @return  Actual number of bytes pushed to buffer
******************************************************************************/
uint16 ringBufPushStaticSrcDma(RingBufCtrl_t *pRbc, const uint8 *pSrc, uint16 numBytes)
{
    uint16 s, n, firstChunk, lastChunk;

    // How many bytes can we transfer before buffer is full?
    s = halIntLock();   // Critical section start (reading 16 bit variable pRbc->numStoredBytes that is altered by an ISR)
    n = MIN(numBytes, GET_FREE_SPACE(pRbc));
    halIntUnlock(s);    // Critical section end

    // Do we need two DMA transfers?
    firstChunk = MIN(n, (pRbc->end - pRbc->head));
    lastChunk = n - firstChunk;

    // 1. First chunk
    // Update DMA configuration table
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].srcAddrH  = HI_UINT16(pSrc);
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].srcAddrL  = LO_UINT16(pSrc);
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].destAddrH = HI_UINT16(pRbc->head);
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].destAddrL = LO_UINT16(pRbc->head);
    pDmaDesc[DMA_STATIC_SRC_CHANNEL].lenL = firstChunk;

    // Wait if channel is currently active
    while (DMA_CHANNEL_ACTIVE( BM(DMA_STATIC_SRC_CHANNEL) ));

    // Load DMA configuration and start the DMA channel
    DMA_CHANNEL_ARM( BM(DMA_STATIC_SRC_CHANNEL) );
    DMA_CHANNEL_TRIG( BM(DMA_STATIC_SRC_CHANNEL) );

    if(lastChunk > 0)
    {
        // 2. Second chunk
        // Wait for the first chunk to complete
        while (DMA_CHANNEL_ACTIVE( BM(DMA_STATIC_SRC_CHANNEL) ));

        // Update DMA configuration table
        pDmaDesc[DMA_STATIC_SRC_CHANNEL].destAddrH = HI_UINT16(pRbc->start);
        pDmaDesc[DMA_STATIC_SRC_CHANNEL].destAddrL = LO_UINT16(pRbc->start);
        pDmaDesc[DMA_STATIC_SRC_CHANNEL].lenL = lastChunk;

        // Load DMA configuration and start the DMA channel
        DMA_CHANNEL_ARM( BM(DMA_STATIC_SRC_CHANNEL) );
        DMA_CHANNEL_TRIG( BM(DMA_STATIC_SRC_CHANNEL) );

        // Update ringbuffer's head value
        pRbc->head = pRbc->start + lastChunk;
    }
    else
    {
        // Only a single DMA transfer was necessary:
        // Update ringbuffer's head value
        pRbc->head += firstChunk;
        // Handle wrap-around
        if (pRbc->head == pRbc->end)
        {
            pRbc->head = pRbc->start;
        }
    }

    // Update number of stored bytes
    s = halIntLock(); // Critical section start (read/modify/write of 16 bit variable that is altered by an ISR)
    pRbc->numStoredBytes += n;
    halIntUnlock(s); // Critical section end

    return n;
} // ringBufPushStaticSrcDma


/**************************************************************************//**
* @fn       ringBufPopByte(RingBufCtrl_t *pRbc, uint8 *pDst)
*
* @brief    Pop single byte from the buffer. Assumes that no other function
*           (e.g. an ISR) pops from buffer at the same time.
*           @warning This function does not check for buffer underrun!
*
* @param    pRbc    Pointer to the ringbuffer controller
* @param    pDst    Pointer to destination for popped byte
*
* @return   none
******************************************************************************/
void ringBufPopByte(RingBufCtrl_t *pRbc, uint8 *pDst)
{
    uint16 s;

    // Copy byte to destination
    *pDst = *pRbc->tail++;

    // Handle wrap-around
    if (pRbc->tail == pRbc->end) {
        pRbc->tail = pRbc->start;
    }

    // Update byte count
    s = halIntLock();   // Critical section start (read/modify/write of 16 bit variable that is altered by an ISR)
    pRbc->numStoredBytes--;
    halIntUnlock(s);    // Critical section end

} // ringBufPopByte


/**************************************************************************//**
* @fn       ringBufPop(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
*
* @brief    Pop bytes from the buffer. Uses CPU (not DMA) for copying.
*
* @param    pRbc        Pointer to the ring buffer controller
* @param    pDst        Pointer to destination array for popped data
* @param    numBytes    Number of bytes to pop
*
* @return   Actual number of bytes popped from buffer
******************************************************************************/
uint16 ringBufPop(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
{
    uint16 s, i, n;

    // Critical section start
    s = halIntLock();

    n = MIN(numBytes, GET_USED_SPACE(pRbc));

    for (i = 0; i < n; i++) {
        pDst[i] = *pRbc->tail++;

        if (pRbc->tail == pRbc->end) {
            pRbc->tail = pRbc->start;
        }
    }
    pRbc->numStoredBytes -= i;

    // Critical section end
    halIntUnlock(s);

    return i;
} // ringBufPop


/**************************************************************************//**
* @fn       ringBufPopStaticDst(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
*
* @brief    Pop bytes from the buffer into a static address, i.e. without
*           incrementing destination address. Uses CPU (not DMA) for copying.
*
* @param    pRbc        Pointer to the ring buffer controller
* @param    pDst        Pointer to destination address for popped data
* @param    numBytes    Number of bytes to pop
*
* @return   Actual number of bytes popped from buffer
******************************************************************************/
uint16 ringBufPopStaticDst(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
{
    uint16 s, i, n;

    // Critical section start
    s = halIntLock();

    n = MIN(numBytes, GET_USED_SPACE(pRbc));

    for (i = 0; i < n; i++) {
        *pDst = *pRbc->tail++;

        if (pRbc->tail == pRbc->end) {
            pRbc->tail = pRbc->start;
        }
    }
    pRbc->numStoredBytes -= i;

    // Critical section end
    halIntUnlock(s);

    return i;
} // ringBufPopStaticDst


/**************************************************************************//**
* @fn       ringBufPopStaticDstDma(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
*
* @brief    Pop bytes from the buffer into a static address, i.e. without
*           incrementing destination address. Uses DMA for copying. Function
*           assumes that no other function (e.g. ISR) pops data to the
*           ringbuffer while it is running. ISR based pushing is ok.
*           @warning This function does not check for buffer underrun!
*           @warning For efficiency, only transfers <256B are supported!
*
* @param    pRbc        Pointer to the ring buffer controller
* @param    pDst        Pointer to destination address for popped data
* @param    numBytes    Number of bytes to pop
*
* @return   Actual number of bytes popped from buffer
******************************************************************************/
uint16 ringBufPopStaticDstDma(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
{
    uint16 s, firstChunk, lastChunk;

    // Do we need 2 DMA transfers?
    firstChunk = MIN(numBytes, (pRbc->end - pRbc->tail));
    lastChunk = numBytes - firstChunk;

    // 1. First chunk
    // Update DMA configuration table
    pDmaDesc[DMA_STATIC_DST_CHANNEL].srcAddrH   = HI_UINT16(pRbc->tail);
    pDmaDesc[DMA_STATIC_DST_CHANNEL].srcAddrL   = LO_UINT16(pRbc->tail);
    pDmaDesc[DMA_STATIC_DST_CHANNEL].destAddrH  = HI_UINT16(pDst);
    pDmaDesc[DMA_STATIC_DST_CHANNEL].destAddrL  = LO_UINT16(pDst);
    pDmaDesc[DMA_STATIC_DST_CHANNEL].lenL       = LO_UINT16(firstChunk);

    // Wait if channel is currently active
    while (DMA_CHANNEL_ACTIVE( BM(DMA_STATIC_DST_CHANNEL) ));

    // Load DMA configuration and start the channel
    DMA_CHANNEL_ARM( BM(DMA_STATIC_DST_CHANNEL) );
    DMA_CHANNEL_TRIG( BM(DMA_STATIC_DST_CHANNEL) );

    if(lastChunk > 0)
    {
        // 2. Second chunk
        // Wait for first chunk to complete
        while (DMA_CHANNEL_ACTIVE( BM(DMA_STATIC_DST_CHANNEL) ));

        // Update DMA configuration table
        pDmaDesc[DMA_STATIC_DST_CHANNEL].srcAddrH   = HI_UINT16(pRbc->start);
        pDmaDesc[DMA_STATIC_DST_CHANNEL].srcAddrL   = LO_UINT16(pRbc->start);
        pDmaDesc[DMA_STATIC_DST_CHANNEL].lenL       = lastChunk;

        // Load DMA configuration and start the channel
        DMA_CHANNEL_ARM( BM(DMA_STATIC_DST_CHANNEL) );
        DMA_CHANNEL_TRIG( BM(DMA_STATIC_DST_CHANNEL) );

        // Update ringbuffer's tail value (implicit wrap-around)
        pRbc->tail = pRbc->start + lastChunk;
    }
    else
    {
        // No second DMA transfer:
        // Update tail value
        pRbc->tail += firstChunk;
        // Ringbuffer wrap-around
        if (pRbc->tail == pRbc->end)
        {
            pRbc->tail = pRbc->start;
        }
    }
    // Update number of stored bytes.
    s = halIntLock();   // Critical section start (read/modify/write of 16 bit variable that is altered by an ISR)
    pRbc->numStoredBytes -= numBytes;
    halIntUnlock(s);    // Critical section end

    return numBytes;
} // ringBufPopStaticDstDma


/**************************************************************************//**
* @fn       ringBufPeek(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
*
* @brief    Read bytes from the buffer, but leave them in the buffer.
*           Uses CPU (not DMA) for copying.
*
* @param    pRbc        Pointer to the ring buffer controller
* @param    pDst        Pointer to destination array for popped data
* @param    numBytes    Number of bytes to peek at
*
* @return   Actual number of peek bytes copied from buffer
******************************************************************************/
uint16 ringBufPeek(RingBufCtrl_t *pRbc, uint8 *pDst, uint16 numBytes)
{
    uint16 s, i, n;
    uint8 *pPeek = pRbc->tail;

    // Critical section start
    s = halIntLock();

    n = MIN(numBytes, GET_USED_SPACE(pRbc));

    for (i = 0; i < n; i++) {
        pDst[i] = *pPeek++;

        if (pPeek == pRbc->end) {
            pPeek = pRbc->start;
        }
    }

    // Critical section end
    halIntUnlock(s);

    return i;
} // ringBufPeek


/**************************************************************************//**
* @fn       ringBufFlush(RingBufCtrl_t *pRbc)
*
* @brief    Flush all the data stored in the buffer.
*
* @param    pRbc    Pointer to the ring buffer controller
*
* @return   none
******************************************************************************/
void ringBufFlush(RingBufCtrl_t *pRbc)
{
    pRbc->tail = pRbc->head;

    uint16 s;

    // Critical section start
    s = halIntLock();

    pRbc->tail = pRbc->head;
    pRbc->numStoredBytes = 0;

    // Critical section end
    halIntUnlock(s);
} // ringBufFlush
/// @}