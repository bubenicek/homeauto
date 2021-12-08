/*******************************************************************************
*  Filename:        hal_mcu.c
*  Revised:         $Date: 2014-06-26 16:14:11 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13250 $
*
*  Description:     Functions for different MCU configurations/commands.
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
#include "hal_types.h"
#include "hal_mcu.h"
#include "clock.h"
#include "hal_defs.h"
#include "hal_assert.h"
#include "power_mode.h"


/*******************************************************************************
* GLOBAL FUNCTIONS
*/
/*******************************************************************************
* @fn           halMcuInit
*
* @brief        Set Main Clock source to XOSC. For the CC2541 and CC2545 enable
*               the 32 kHz XTAL as sleep timer clock source for better precision.
*
* @param        void
*
* @return       void
*/
void halMcuInit(void) {
#if((chip == 2541) | (chip == 2545))
    // Disable the 32 kHz RCOSC calibration.
    SLEEPCMD = 0x80;
    // The 16 MHz RCOSC must be system clock before enabling the 32 kHz XTAL.
    clockSetMainSrc(CLOCK_SRC_HFRC);
    // Enable 32 kHZ XTAL.
    clockSelect32k(CLOCK_32K_XTAL);
#endif
    /*  Enable the 32 MHz XTAL. THe 32 kHz RCOSC calibration is enabled by
    *   default and this setting takes effect after the device has run from
    *   16 MHz RCOSC as system clock (Default after reset or poewr on). */
    clockSetMainSrc(CLOCK_SRC_XOSC);
    // 32 kHz RCOSC calibration starts here and last for approx 2 ms.
}


/*******************************************************************************
* @fn           halMcuWaitUs
*
* @brief        Busy wait function. Waits the specified number of microseconds.
*               Use assumptions about number of clock cycles needed for the
*               various instructions. This function assumes a 32 MHz clock.
*
* @NB           This function is highly dependent on architecture and compiler!
*
* @param        uint16 usec - number of microseconds delays
*
* @return       void
*/
#pragma optimize=none
void halMcuWaitUs(uint16 usec) {
    usec>>= 1;
    while(usec--) {
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
    }
}


/*******************************************************************************
* @fn           halMcuWaitMs
*
* @brief        Busy wait function. Waits the specified number of milliseconds.
*               Use assumptions about number of clock cycles needed for the
*               various instructions.
*
* @NB           This function is highly dependent on architecture and compiler!
*
* @param        uint16 millisec - number of milliseconds delay
*
* @return       void
*/
#pragma optimize=none
void halMcuWaitMs(uint16 msec) {
    while(msec--)
        halMcuWaitUs(1000);
}



/*******************************************************************************
* @fn           halMcuReset
*
* @brief        Resets the MCU. This utilize the watchdog timer as there is no
*               other way for a software reset. The reset will not occur until
*               ~2 ms.
*
* @NB           The function will not return! (hangs until reset)
*
* @param        uint16 millisec - number of milliseconds delay
*
* @return       void
*/
void halMcuReset(void) {
    const uint8 WDT_INTERVAL_MSEC_2=                    0x03;   // after ~2 ms
    WDCTL = ((WDCTL & 0xFC) | (WDT_INTERVAL_MSEC_2 & 0x03));
    // Start watchdog
    WDCTL &= ~0x04;     // Select watchdog mode
    WDCTL |= 0x08;      // Enable timer
    while(1);           // Halt here until reset
}


/*******************************************************************************
* @fn           halMcuWdInit
*
* @brief        Initialise the watchdog timer
*
*
* Parameters:   uint8 interval:
*               0x03 -    1.9   ms
*               0x02 -   15.625 ms
*               0x01 -  250     ms
*               0x00 - 1000     ms
*
* @return       void
*/
void halMcuWdInit(uint8 interval) {
    // Select timer interval
    WDCTL = (WDCTL & ~0x03) | (interval & 0x03);
    // Start watchdog
    WDCTL &= ~0x04;     // Select watchdog mode
    WDCTL |= 0x08;      // Enable timer
}


/*******************************************************************************
* @fn           halMcuWdClear
*
* @brief        Clear the watchdog timer
*
*
* @param        void
*
* @return       void
*/
void halMcuWdClear(void) {
    WDCTL= (WDCTL & 0x0F) | 0xA0;
    WDCTL= (WDCTL & 0x0F) | 0x50;
}