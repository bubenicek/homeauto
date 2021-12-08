/*******************************************************************************
*  Filename:        clock.c
*  Revised:         $Date: 2014-06-26 16:14:11 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13250 $
*
*  Description:     This file defines clock related functions for the CC254x
*                   family of RF system-on-chips from Texas Instruments.
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
#include "clock.h"
#include "hal_defs.h"
#include "hal_mcu.h"
#include "ioCC254x_bitdef.h"


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
/*******************************************************************************
* @fn       clockSetMainSrc
*
* @brief    Function for setting the main system clock source.
*           The function turns off the clock source that is not being used.
*           TICKSPD is set to the same frequency as the source.
*
* @param    uint8 source: Value to set in CLKCONCMD register.
*
* @return   void
*
*******************************************************************************/
void clockSetMainSrc(uint8 source) {
    uint8 rcOsc = 0;
#if(chip==2543 | chip==2544)
    rcOsc = 0x80;
#endif
    if (source == CLOCK_SRC_HFRC){
        CLKCONCMD = 0xC9 | rcOsc;
    }
    else if (source == CLOCK_SRC_XOSC){
//        CLKCONCMD &= ~(CLKCON_CLKSPD | CLKCON_TICKSPD | CLKCON_OSC);
        CLKCONCMD = 0x80; // 32 kHz RCOSC
    }
    else if (source == CLOCK_SRC_XOSC_250kHz){
        CLKCONCMD = 0x3F | rcOsc;
    }
    else if (source == CLOCK_SRC_XOSC_2MHz){
        CLKCONCMD = 0x24 | rcOsc;
    }
    else if (source == CLOCK_SRC_XOSC_4MHz){
        CLKCONCMD = 0x1B | rcOsc;
    }
    else if (source == CLOCK_SRC_XOSC_8MHz){
//        CLKCONCMD = 0x12 | rcOsc;
        CLKCONCMD = 0x3A | rcOsc;  // Set timer ticks to max division (250 kHz)
    }
    else if (source == CLOCK_SRC_XOSC_16MHz){
        CLKCONCMD = 0x09 | rcOsc;
    }
    CC254x_WAIT_CLK_UPDATE();
    return;
}


/*******************************************************************************
* @fn       clockSetMainClkSpeed
*
* @brief    Function for setting the main system clock speed.
*
* @param    uint8 clkSpeed: Value to set in CLKCONCMD register.
*
* @note     The debugger cannot be used with a divided system clock. When
*           running the debugger, the value of CLKCONCMD.CLKSPD should be set
*           to 000 when CLKCONCMD.OSC = 0 or to 001 when CLKCONCMD.OSC = 1.
*
* @return   void
*
*******************************************************************************/
uint8 clockSetMainClkSpeed(uint8 clkSpeed) {
    uint8 lowerLimit = 0;

    //  Check if tick speed is set higher than source clock speed......
    if(!(CLKCONSTA & CLKCON_OSC)) {
        lowerLimit = 1;
    }

    // Make sure no other values in CLKCONCMD will be set.
    if(lowerLimit <= (clkSpeed &= CLKCON_CLKSPD) <= 7) {
        CLKCONCMD &= ~CLKCON_CLKSPD;
        // Also set tick speed to same speed as clock speed.
        CLKCONCMD |= (clkSpeed | ((clkSpeed << 3) & CLKCON_TICKSPD));
    }
    else {
        // Illegal value used.
        return FAILED;
    }
    CC254x_WAIT_CLK_UPDATE();
    return SUCCESS;
}


/*******************************************************************************
* @fn       clockSelect32k
*
* @brief    Function for selecting source for the 32kHz oscillator
*
* @param    uint8 source (one of CLOCK_32K_XTAL or CLOCK_32K_RCOSC)
*
* @note     Only CC2541 and CC2545 has the external 32kHz XTAL option.
*
* @return   uint8 - SUCCESS or FAILED
*
*******************************************************************************/
uint8 clockSelect32k(uint8 source)
{
#if((chip == 2541) || (chip == 2545))
    /*  System clock source must be high frequency RC oscillator before
    *   changing 32K source.  */
    if( !(CLKCONSTA & CLKCON_OSC_BM) )
      return FAILED;

    if (source == CLOCK_32K_XTAL){
        CLKCONCMD &= ~CLKCON_OSC32K_BM;
    }
    else if (source == CLOCK_32K_RCOSC){
        CLKCONCMD |= CLKCON_OSC32K_BM;
    }
#elif((chip == 2543) || (chip == 2544))
    // CC2543/44 only has a internal 32 KHz RCOSC.
    CLKCONCMD |= CLKCON_OSC32K_BM;
#endif

    CC254x_WAIT_CLK_UPDATE();

    return SUCCESS;
}