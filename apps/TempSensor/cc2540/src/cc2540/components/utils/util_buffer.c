/*******************************************************************************
*  Filename:        util_buffer.c
*  Revised:         $Date: 2014-06-26 16:17:44 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13251 $
*
*  Description:     Ringbuffer implementation.
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
#include "util_buffer.h"
#include "hal_int.h"
#include "hal_assert.h"


/*******************************************************************************
* GLOBAL FUNCTIONS
*/

/*******************************************************************************
* @fn      bufInit
*
* @brief   Initialise a ringbuffer. The buffer must be allocated by the
*          application.
*
* @param   pBuf - pointer to the ringbuffer
*
* @return  none
*/
void bufInit(ringBuf_t *pBuf) {
    uint16 s;

    // Critical section start
    s = halIntLock();

    pBuf->nBytes= 0;
    pBuf->iHead= 0;
    pBuf->iTail= 0;

    // Critical section end
    halIntUnlock(s);
}


/*******************************************************************************
* @fn      bufPut
*
* @brief   Add bytes to the buffer.
*
* @param   pBuf - pointer to the ringbuffer
*          pData - pointer to data to be appended to the buffer
*          nBytes - number of bytes
*
* @return  Number of bytes copied to the buffer
*/
uint8 bufPut(ringBuf_t *pBuf, const uint8 *pData, uint8 nBytes) {
    uint8 i;
    uint16 s;

    // Critical section start
    s = halIntLock();

    if (pBuf->nBytes+nBytes < BUF_SIZE) {

        i= 0;
        while(i<nBytes) {
            pBuf->pData[pBuf->iTail]= pData[i];
            pBuf->iTail++;
            if (pBuf->iTail==BUF_SIZE)
                pBuf->iTail= 0;
            i++;
        }
        pBuf->nBytes+= i;
    } else {
        i= 0;
    }

    // Critical section end
    halIntUnlock(s);

    return i;
}


/*******************************************************************************
* @fn      bufGet
*
* @brief   Extract bytes from the buffer.
*
* @param   pBuf   - pointer to the ringbuffer
*          pData  - pointer to data to be extracted
*          nBytes - number of bytes
*
* @return  Bytes actually returned
*/
uint8 bufGet(ringBuf_t *pBuf, uint8 *pData, uint8 nBytes) {
    uint8 i;
    uint16 s;

    // Critical section start
    s = halIntLock();

    i= 0;
    while(i<nBytes && i<pBuf->nBytes) {
        pData[i]= pBuf->pData[pBuf->iHead];
        pBuf->iHead++;
        if (pBuf->iHead==BUF_SIZE)
            pBuf->iHead= 0;
        i++;
    }
    pBuf->nBytes-= i;

    // Critical section end
    halIntUnlock(s);
    return i;
}


/*******************************************************************************
* @fn      bufPeek
*
* @brief   Read bytes from the buffer but leave them in the queue.
*
* @param   pBuf   - pointer to the ringbuffer
*          pData  - pointer to data to be extracted
*          nBytes - number of bytes
*
* @return  Bytes actually returned
*/
uint8 bufPeek(ringBuf_t *pBuf, uint8 *pData, uint8 nBytes) {
    uint8 i,j;
    uint16 s;

    // Critical section start
    s = halIntLock();

    i= 0;
    j= pBuf->iHead;
    while(i<nBytes && i<pBuf->nBytes) {
        pData[i]= pBuf->pData[j];
        j++;
        if (j==BUF_SIZE)
            j= 0;
        i++;
    }

    // Critical section end
    halIntUnlock(s);
    return i;
}


/*******************************************************************************
* @fn      bufNumBytes
*
* @brief   Return the byte count for the ring buffer.
*
* @param   pBuf- pointer to the buffer
*
* @return  Number of bytes present.
*/
uint8 bufNumBytes(ringBuf_t *pBuf) {
    return pBuf->nBytes;
}