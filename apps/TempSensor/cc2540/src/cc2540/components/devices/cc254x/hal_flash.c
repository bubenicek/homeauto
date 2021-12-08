/******************************************************************************
*   Filename:       hal_flash.c
*   Revised:        $Date: 2014-06-13 20:06:09 +0200 (fr, 13 jun 2014) $
*   Revision:       $Revision: 113 $
*
*   Description:    This file defines flash read and write functions for the
*                   CC254x family of RF system-on-chips from Texas Instruments.
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
#include <dma.h>
#include "hal_board_cfg.h"
#include "hal_flash.h"
#include "hal_types.h"
#include "hal_int.h"
#include "ioCC254x_bitdef.h"


// DMA configuration descriptor used for flash write.
static DMA_DESC dmaConfig0;


/*******************************************************************************
 * @fn          HalFlashRead
 *
 * @brief       This function reads 'cnt' bytes from the internal flash.
 *
 * input parameters
 *
 * @param       pg - A valid flash page number.
 * @param       offset - A valid offset into the page.
 * @param       buf - A valid buffer space at least as big as the 'cnt' parameter.
 * @param       cnt - A valid number of bytes to read.
 *
 * output parameters
 *
 * None.
 *
 * @return      Void
 */
void HalFlashRead(uint8 pg, uint16 offset, uint8 *buf, uint16 cnt) {
    // Calculate the offset into the containing flash bank as it gets mapped into XDATA.
    uint8 *ptr = (uint8 *)(offset + HAL_FLASH_PAGE_MAP) +
        ((pg % HAL_FLASH_PAGE_PER_BANK) * HAL_FLASH_PAGE_SIZE);
    uint8 memctr = MEMCTR;  // Save to restore.

    unsigned char IntState;
    pg /= HAL_FLASH_PAGE_PER_BANK;  // Calculate the flash bank from the flash page.

    HAL_INT_LOCK(IntState);         // Enter critical section.
    // Calculate and map the containing flash bank into XDATA.
    MEMCTR = (MEMCTR & 0xF8) | pg;
    while (cnt--) {
        *buf++ = *ptr++;
    }
    MEMCTR = memctr;
    HAL_INT_UNLOCK(IntState);       // Exit critical section.
    return;
}


/**************************************************************************************************
 * @fn          HalFlashWrite
 *
 * @brief       This function writes 'cnt' bytes to the internal flash.
 *
 * input parameters
 *
 * @param       addr - Valid HAL flash write address: actual addr / 4 and quad-aligned.
 * @param       buf - Valid buffer space at least as big as 'cnt' X 4.
 * @param       cnt - Number of 4-byte blocks to write.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void HalFlashWrite(uint16 addr, uint8 *buf, uint16 cnt) {

    /* The DMA configuration data structure may reside at any location in
    * unified memory space, and the address location is passed to the DMA
    * through DMA0CFGH:DMA0CFGL. */
    DMA0CFGH = ((unsigned short)&dmaConfig0 >> 8) & 0x00FF;
    DMA0CFGL = (unsigned short)&dmaConfig0 & 0x00FF;

    /* Configure DMA channel 0:
     * SRCADDR:   address of the data to be written to flash (increasing).
     * DESTADDR:  the flash controller data register (fixed), so that the
     *            flash controller will write this data to flash.
     * VLEN:      use LEN for transfer count.
     * LEN:       equal to the number of bytes to be transferred.
     * WORDSIZE:  each transfer should transfer one byte.
     * TMODE:     should be set to single mode (see datasheet, DMA Flash Write).
     *            Each flash write complete will re-trigger the DMA channel.
     * TRIG:      let the DMA channel be triggered by flash data write complete
     *            (trigger number 18). That is, the flash controller will trigger
     *            the DMA channel when the Flash Write Data register, FWDATA, is
     *            ready to receive new data.
     * SRCINC:    increment by one byte.
     * DESTINC:   fixed (always write to FWDATA).
     * IRQMASK:   disable interrupts from this channel.
     * M8:        0, irrelevant since we use LEN for transfer count.
     * PRIORITY:  high.
     */
    dmaConfig0.srcAddrH     = ((unsigned short)buf >> 8) & 0x00FF;
    dmaConfig0.srcAddrL     = (unsigned short)buf & 0x00FF;
    dmaConfig0.destAddrH    = ((unsigned short)&FWDATA >> 8) & 0x00FF;
    dmaConfig0.destAddrL    = (unsigned short)&FWDATA & 0x00FF;
    dmaConfig0.vlen         = DMA_VLEN_USE_LEN;
    dmaConfig0.lenH         = (uint8) ((cnt >> 8) & 0x00FF);
    dmaConfig0.lenL         = (uint8) (cnt & 0x00FF);
    dmaConfig0.wordSize     = DMA_WORDSIZE_BYTE;
    dmaConfig0.tMode        = DMA_TMODE_SINGLE;
    dmaConfig0.trig         = DMA_TRIG_FLASH;
    dmaConfig0.srcInc       = DMA_SRCINC_1;
    dmaConfig0.destInc      = DMA_DESTINC_0;
    // The DMA is to be polled and shall not issue an IRQ upon completion.
    dmaConfig0.irqMask      = DMA_IRQMASK_DISABLE;
    dmaConfig0.m8           = DMA_M8_USE_8_BITS;
    dmaConfig0.priority     = DMA_PRI_HIGH;

    // Waiting for the flash controller to be ready.
    while (FCTL & FCTL_BUSY);

    // Arm the DMA channel, takes 9 system clock cycles.
    DMAARM |= DMAARM_DMAARM0;

    FADDRH = (addr >> 8) & 0x00FF;
    FADDRL = addr & 0x00FF;

    // Enable flash write. Generates a DMA trigger.
    FCTL |= FCTL_WRITE;

    // Wait until Flash write is complete.
    while (FCTL & FCTL_BUSY);
    return;
}


void HalFlashWritePage(uint16 addr, uint8 *buf, uint16 cnt) 
{

    /* Configure DMA channel 0:
     * SRCADDR:   address of the data to be written to flash (increasing).
     * DESTADDR:  the flash controller data register (fixed), so that the
     *            flash controller will write this data to flash.
     * VLEN:      use LEN for transfer count.
     * LEN:       equal to the number of bytes to be transferred.
     * WORDSIZE:  each transfer should transfer one byte.
     * TMODE:     should be set to single mode (see datasheet, DMA Flash Write).
     *            Each flash write complete will re-trigger the DMA channel.
     * TRIG:      let the DMA channel be triggered by flash data write complete
     *            (trigger number 18). That is, the flash controller will trigger
     *            the DMA channel when the Flash Write Data register, FWDATA, is
     *            ready to receive new data.
     * SRCINC:    increment by one byte.
     * DESTINC:   fixed (always write to FWDATA).
     * IRQMASK:   disable interrupts from this channel.
     * M8:        0, irrelevant since we use LEN for transfer count.
     * PRIORITY:  high.
     */
    dmaConfig0.srcAddrH     = ((unsigned short)buf >> 8) & 0x00FF;
    dmaConfig0.srcAddrL     = (unsigned short)buf & 0x00FF;
    dmaConfig0.destAddrH    = ((unsigned short)&FWDATA >> 8) & 0x00FF;
    dmaConfig0.destAddrL    = (unsigned short)&FWDATA & 0x00FF;
    dmaConfig0.vlen         = DMA_VLEN_USE_LEN;
    dmaConfig0.lenH         = (uint8) ((cnt >> 8) & 0x00FF);
    dmaConfig0.lenL         = (uint8) (cnt & 0x00FF);
    dmaConfig0.wordSize     = DMA_WORDSIZE_BYTE;
    dmaConfig0.tMode        = DMA_TMODE_SINGLE;
    dmaConfig0.trig         = DMA_TRIG_FLASH;
    dmaConfig0.srcInc       = DMA_SRCINC_1;
    dmaConfig0.destInc      = DMA_DESTINC_0;
    // The DMA is to be polled and shall not issue an IRQ upon completion.
    dmaConfig0.irqMask      = DMA_IRQMASK_DISABLE;
    dmaConfig0.m8           = DMA_M8_USE_8_BITS;
    dmaConfig0.priority     = DMA_PRI_HIGH;
  
    /* The DMA configuration data structure may reside at any location in
     * unified memory space, and the address location is passed to the DMA
     * through DMA0CFGH:DMA0CFGL.
     */
    DMA0CFGH = ((uint16)&dmaConfig0 >> 8) & 0x00FF;
    DMA0CFGL = (uint16)&dmaConfig0 & 0x00FF;
  
    // Waiting for the flash controller to be ready. 
    while (FCTL & FCTL_BUSY);
  
    /* Configuring the flash controller. 
     * FADDRH:FADDRL: point to the area in flash to write to.
     */
    addr = (uint16)addr >> 2;    // You address 32-bit words through the flash controller.
    
    FADDRH = (addr >> 8) & 0x00FF;
    FADDRL = addr & 0x00FF;
  
    // Erase the page that will be written to.
    FCTL |= FCTL_ERASE;            
  
    // Wait for the erase operation to complete.
    while (FCTL & FCTL_BUSY);
  
    // Arm the DMA channel, takes 9 system clock cycles.
    DMAARM |= DMAARM_DMAARM0;
    NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP();NOP(); // 9 NOPs
  
    // Enable flash write. Generates a DMA trigger.
    FCTL |= FCTL_WRITE;
  
    // Wait for DMA transfer to complete.
    while (!(DMAIRQ & DMAIRQ_DMAIF0));
  
    // Wait until flash controller not busy.
    while (FCTL & (FCTL_BUSY | FCTL_FULL));
  
    /* By now, the transfer is completed, so the transfer count is reached.
     * The DMA channel 0 interrupt flag is then set, so we clear it here.
     */
    DMAIRQ = ~DMAIRQ_DMAIF0;      // Clear interrupt flag by R/W0, see datasheet. 
}


/*******************************************************************************
 * @fn          HalFlashErase
 *
 * @brief       This function erases the specified page of the internal flash.
 *
 * input parameters
 *
 * @param       pg - A valid flash page number to erase.
 *
 * @return      void
 */
void HalFlashErase(uint8 pg) 
{
#if 0   
    FADDRH = pg * (HAL_FLASH_PAGE_SIZE / HAL_FLASH_WORD_SIZE / 256);
    FCTL |= 0x01;
    while (FCTL & 0x80); /* optional: wait until flash write has completed (~20 ms) */
#else
    /* Erase one flash page */
    EA = 0; /* disable interrupts */
    while (FCTL & 0x80); /* poll FCTL.BUSY and wait until flash controller is ready */
    FADDRH = pg << 1; /* select the flash page via FADDRH[6:0] bits */
    FCTL |= 0x01; /* set FCTL.ERASE bit to start page erase */
    while (FCTL & 0x80); /* optional: wait until flash write has completed (~20 ms) */
    EA = 1; /* enable interrupts */  
#endif   
}