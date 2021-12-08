/*******************************************************************************
*  Filename:        hal_sleep.c
*  Revised:         $Date: 2013-04-24 18:02:21 +0200 (on, 24 apr 2013) $
*  Revision:        $Revision: 9911 $
*
*  Description:     Contains sleep and power mode related functions.
*
*  Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
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


/*******************************************************************************
 * INCLUDES
 */
#include "clock.h"
#include "hal_defs.h"
#include "hal_mcu.h"
#include "power_mode.h"
#include "hal_rf_proprietary.h"
#include "hal_sleep.h"
#include "hal_types.h"
#include "ioCC254x_bitdef.h"
#include "hal_timer2.h"


/*******************************************************************************
* GLOBAL VARIABLES
*/
// Union for storing 24 bit sleep timer value.
typedef union {
    unsigned long value;
    uint8 byte[4];
} sleepTime_t;
static sleepTime_t sleepTime;
static sleepTime_t sleepTime1;
static sleepTime_t sleepTime2;
uint8 sleepTimerCompareFlag;


/*******************************************************************************
 * GLOBAL FUNCTIONS
 */
/*******************************************************************************
* @fn          sleep_isr
*
* @brief       Interrupt service routine for the sleep timer which wakes the
*              system from Power Mode. When awake the flags are cleared and
*              LED1 is toggled.
*
* @param       void
*
* @return      void
*/
#pragma vector = ST_VECTOR
__interrupt void sleep_isr(void) {
    // Clear [IRCON.STIF] (Sleep Timer CPU interrupt flag).
    STIF = 0;
}


/*******************************************************************************
* @fn           halSleepEnterPowerMode
*
* @brief        Set device into PM0, PM1, PM2 or PM3.
*
* @param        uint8 powerMode:
*               0 - Enter PM0
*               1 - Enter PM1
*               2 - Enter PM2 (NA on CC2544)
*               3 - Enter PM3 (NA on CC2544)
*
* @return      void
*/
void halSleepEnterPowerMode(uint8 powerMode) {
#if(chip==2544)
  if(powerMode) {
        powerMode = 1;
  }
#endif
  
    // Set power mode.
    SLEEPCMD = ((SLEEPCMD & ~SLEEPCMD_MODE) | powerMode);

    if(powerMode > 1) {
        // Set LLE in Reset to avoid unknown state on wake-up.
        LLECTRL &= ~LLECTRL_LLE_EN;
    }
    else {
        // Going to halt CPU, dividide down system clock to reduce current consumption.
        clockSetMainClkSpeed(CLKCON_CLKSPD_2M);
    }
    
    /* Enter powermode
    * Sets PCON.IDLE with a 2-byte boundary assembly instruction.
    * NOTE: Cache must be enabled (see CM in FCTL).
    * This method gives the least current consumption. */
    EnterSleepModeProcessInterruptsOnWakeup();

    if(powerMode > 1) {
        // Re-Enable the radio.
        LLECTRL |= LLECTRL_LLE_EN;
    }
    
    /* Make sure that system clock is 32 MHz without division after wakeup.
    * This function will also wait until the 32 MHz clock is stable */
    clockSetMainSrc(CLOCK_SRC_XOSC);
}


/*******************************************************************************
* @fn           halSleepSetSleepTimer
*
* @brief        Set the sleep timer compare value for wakeup from power mode.
*
* @param        uint32 sleep_duration: Number of 32 kHz ticks to sleep.
*
* @return       void
*/
void halSleepSetSleepTimer(uint32 sleep_duration) {
    // To ensure an updated value is read, wait for a positive transition on the
    // 32 kHz clock by polling the SLEEPSTA.CLK32K bit.
    while(!(SLEEPSTA & 0x01)); // Wait for positive flank on sleep timer. 
    
    // Read the sleep timer current value. 
    sleepTime.byte[0] = ST0;    // ST0 must be read first.
    sleepTime.byte[1] = ST1;                  
    sleepTime.byte[2] = ST2;
    sleepTime.byte[3] = 0;

    // Add the duration to the Sleep Timer value.
    sleepTime.value += sleep_duration;

    // When loading a new compare value for the Sleep Timer,
    // the ST2 and ST1 registers must be loaded before ST0. 
    ST2 = sleepTime.byte[2];
    ST1 = sleepTime.byte[1];
            
    // Wait until load ready, before writing to STO.
    while( !(STLOAD & 0x01) );
    ST0 = sleepTime.byte[0];
}


/*******************************************************************************
* @fn           halSleepSetSleepTimer
*
* @brief        Set the sleep timer compare value for wakeup from power mode.
*
* @param        sleepTime_t *sleepTime: Pointer to sleep time struct.
*
* @return       void
*/
void halSleepReadTimer(sleepTime_t *sleepTime) {
    /* To ensure an updated value is read, wait for a positive transition on the
    *  32 kHz clock by polling the SLEEPSTA.CLK32K bit. */
    while(!(SLEEPSTA & 0x01)); // Wait for positive flank on sleep timer.

    // Read the sleep timer current value.
    sleepTime->byte[0] = ST0;    // ST0 must be read first.
    sleepTime->byte[1] = ST1;
    sleepTime->byte[2] = ST2;
    sleepTime->byte[3] = 0;
}


/*******************************************************************************
* @fn           halSleepEnableInterrupt
*
* @brief        Enable Sleep Timer Interrupt.
*
* @param        void
*
* @return       void
*/
void halSleepEnableInterrupt(void) {
    // Clear the sleep timer interrupt flag.
    STIF = 0;
    // Enable the sleep timer interrupt.
    STIE = 1;
}


/*******************************************************************************
* @fn           halSleepDisableInterrupt
*
* @brief        Enable Sleep Timer Interrupt.
*
* @param        void
*
* @return       void
*/
void halSleepDisableInterrupt(void) {
    // Clear the sleep timer interrupt flag.
    STIF = 0;
    // Enable the sleep timer interrupt.
    STIE = 0;
}


/*******************************************************************************
* @fn           halSleepSynchXOSCMsBase
*
* @brief        Sync Timer2 to the internal 32 kHz RCOSC sleep timer. Require 
*               the 32 kHz RCOSC to be calibrated (+/- 0.4 % of 32.753 kHz when 
*               Calibrated). 
*               
* @warning      This method is not 100% accurate (read comments in function for 
*               more details). The function also assume that the timer2 base 
*               period is set to 1 ms.
*
* @param        uint8 start:
*               true: update and start timer2.
*               false: Stop timer2 and store the sleep timer value.
*
* @return       void
*/
void halSleepSynchXOSCMsBase(uint8 start) {
    uint32 sleepTimeDiff  = 0;
    uint16 baseTicks = 0, overFlowTicks  = 0;

    if(!start) {
        // Clear timstamps.
        sleepTime1.value = sleepTime2.value = 0;

        // Stop timer 2.
        halTimer2Stop(0);

        // Read current ST value.
        halSleepReadTimer(&sleepTime1);
    }
    else {
        halSleepReadTimer(&sleepTime2);
        
        /* Trim/tune START. */
        
        /*  This function assumes that the timer 2 base period is set to 1 ms. 
        *   During one ms the low speed RC oscillator has ticked 32.768 times. */

        // Find sleep time elapsed.
        sleepTimeDiff = sleepTime2.value-sleepTime1.value;
        
        /*  Add the execution time (roughly) between Trim/tune START /\ and END \/
        *   See comments further up and further down. The execution time will 
        *   vary slightly depending on the numbers used for the calculations 
        *   below and the optimaztion level. To acquire higher accuracy, this 
        *   function should be written in assembler and tuned to have a 
        *   deterministic results. */

        // Add execution time (roughly around 160 us).
        sleepTimeDiff += 5;

        // Divide by 32 to get overflow periods in 1 ms ....
        overFlowTicks = ((sleepTimeDiff >> 5) & 0x07FFFFFF);
    
        /* Multiply the remainder (sleepTimeDiff %= 32) by 977 to get
        * number of 31.25 ns base periods */
        baseTicks = (((uint16) (sleepTimeDiff & 0x0000001F)) * 977);

        // Update Timer 2 with current time, use 1 ms base period (=0x7D00 ticks).
        halTimer2UpdateTimerCount(baseTicks, overFlowTicks, 0x7D00, 0);
        
        // Start timer 2.
        halTimer2Start(0);
        
        /* Trim/tune END. */
  }
  return;
}

/**************************************************************************************************
 * @fn          halSleepWait
 *
 * @brief       Perform a blocking wait for the specified number of microseconds.
 *              Use assumptions about number of clock cycles needed for the various instructions.
 *              This function assumes a 32 MHz clock.
 *              NB! This function is highly dependent on architecture and compiler!
 *
 * input parameters
 *
 * @param       duration - Duration of wait in microseconds.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 **************************************************************************************************
 */
#pragma optimize=none
void halSleepWait(uint16 duration)
{
  duration >>= 1;
    
  while (duration-- > 0)
  {
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
    NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP(); NOP();
  }
}
