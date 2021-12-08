/*******************************************************************************
*  Filename:        hal_timer2.c
*  Revised:         $Date: 2014-02-06 16:28:25 +0100 (to, 06 feb 2014) $
*  Revision:        $Revision: 12078 $
*
*  Description:     HAL for radio timer Timer 2.
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
#include "hal_timer2.h"
#include "hal_int.h"
#include "prop_regs.h"
#include "ioCC254x_bitdef.h"

// Include device specific file
#if (chip==2541)
#include "ioCC2541.h"
#elif (chip==2543)
#include "ioCC2543.h"
#elif (chip==2544)
#include "ioCC2544.h"
#elif (chip==2545)
#include "ioCC2545.h"
#else
#error "Chip not supported!"
#endif


/*******************************************************************************
* GLOBAL VARIABLES
*/
uint8 RadioTimeoutFlag;


/*******************************************************************************
* GLOBAL FUNCTIONS
*/
/*******************************************************************************
* @fn          sleep_isr
*
* @brief       Interrupt service routine for timer2. Clears all flags.
*
* @param       void
*
* @return      void
*/
HAL_ISR_FUNCTION(T2_ISR,T2_VECTOR) {
    uint8 t2irqf = T2IRQF;
    if(t2irqf & TIMER2_OVF_COMPARE1F) {
        RadioTimeoutFlag = 1;
    }
    // Clear All Timer2 Interrupt flags.
    T2IRQF = ~t2irqf;
    // Clear [IRCON.T2IF] ( Timer 2 interrupt flag ).
    T2IF = 0;
}


/*******************************************************************************
* @fn           halTimer2EnableInterrupt
*
* @brief        Enable Timer 2 interrupt source.
*
* @param        uint8 t2irqmSelection:
*               value:  The selected Timer 2 Interrupt Mask.
*               0:      Only enable Timer2 general interrupt
*
* @return       void
*/
void halTimer2EnableInterrupt(uint8 t2irqmSelection) {
    // Enable the selected T2 interrupt bit mask.
    T2IRQM |= t2irqmSelection;
    // Set Timer 2 interrupt enable.
    T2IE = 1;
}


/*******************************************************************************
* @fn           halTimer2DisableInterrupt
*
* @brief        Disable Timer 2 interrupt source.
*
* @param        uint8 t2irqmSelection:
*               value:  The selected Timer 2 Interrupt Mask.
*               0:      Only enable Timer2 general interrupt
*
* @return       void
*/
void halTimer2DisableInterrupt(uint8 t2irqmSelection) {
    // Disable the selected T2 interrupt bit mask.
    T2IRQM &= ~t2irqmSelection;
    // Disable Timer 2 interrupt.
    T2IE = 0;
}


/*******************************************************************************
* @fn           halTimer2SetRadioTimeout
*
* @brief        Set timeout for radio with timer 2.
*
* @param        uint32 ms:
*               Number of ms (range 0 to 16777216 (4. 66 hours))
*
* @param        uint8 periodic:
*               1 - Set up a periodic timeout signal.
*               0 - Set up a single timeout signal.
*
* @return       void
*/
void halTimer2SetRadioTimeout(uint32 ms, uint8 periodic) {
    uint8 IntState;
    HAL_INT_LOCK(IntState);     // Start Critical section.

    // Clear timer count and overflow value.
    halTimer2Reset();

    // Clear timeout flag.
    RadioTimeoutFlag = 0;

    if(periodic) {
        // Enables the TIMER2_OVF_PER interrupt.
        halTimer2EnableInterrupt(T2IRQM_OVF_PERM);
        // Modify timer 2 period and overflow period.
        T2MSEL = 0x22;
        // Set timer 2 event 2 to overflow period and disable event 1.
        T2EVTCFG = 0x37;
    }
    else {
        // Enables the T2IRQM_OVF_COMPARE1M interrupt.
        halTimer2EnableInterrupt(T2IRQM_OVF_COMPARE1M);
        // Set timer period (t2_per) and overflow compare 1 (t2ovf_cmp1).
        T2MSEL = 0x32;
        // Set timer 2 event 2 to overflow long compare 1 and disable event 1.
        T2EVTCFG = 0x47;
    }

    // Set basic period to 1.0 ms.
    T2M0 = 0x00;
    T2M1 = 0x7D;
    // Overflow compare value.
    T2MOVF0 = (uint8) (ms & 0xFF);
    T2MOVF1 = (uint8) ((ms >> 8) & 0xFF);
    T2MOVF2 = (uint8) ((ms >> 16) & 0xFF);

    // Stop radio on timer 2 event 2.
    PRF.TASK_CONF.STOP_CONF   = 1;

    // Clear timer 2 IRQs.
    T2IRQF = 0;
    T2IF = 0;
    HAL_INT_UNLOCK(IntState);   // End Critical section.
}


/*******************************************************************************
 * @fn          halTimer2GetFullCurrentTime
 *
 * @brief       This routine is used to retrieve the current value of the
 *              Timer 2 24-bit Overflow counter and the 16-bit Timer counter.
 *
 *              Note: This routine assumes Timer 2's LATCH_MODE is set to one;
 *                    i.e. reading T2M0 latches T2M1 and T2MOVF0-T2MOVF2.
 *
 * @param       uint32* coarse - Coarse system time in 625us ticks.
 * @param       uint16* fine   - Fine system time in 31.25ns ticks.
 *
 * @return      void
 */
void halTimer2GetFullCurrentTime(uint16 *fine, uint32 *coarse) {
    // Select T2 Timer and Overflow 24-bit counter.
    T2MSEL = T2MSEL_T2MSEL_TIMER_COUNT_VALUE | T2MSEL_T2MOVFSEL_OVERFLOW_COUNTER;

    if(fine) {
        // Read Timer to latch T2M1 and T2MOVF0-T2MOVF2.
        ((uint8 *)fine)[0] = T2M0;
        ((uint8 *)fine)[1] = T2M1;
    }

    if(coarse) {
        // Read overflow counter.
        ((uint8 *)coarse)[0] = T2MOVF0;
        ((uint8 *)coarse)[1] = T2MOVF1;
        ((uint8 *)coarse)[2] = T2MOVF2;
        ((uint8 *)coarse)[3] = 0;
    }
  return;
}


/*******************************************************************************
 * @fn          halTimer2GetCapturedTime
 *
 * @brief       This routine is used to retrieve the current value of the
 *              Timer 2 24-bit Overflow counter and the 16-bit Timer counter.
 *
 *              Note: This routine assumes Timer 2's LATCH_MODE is set to one;
 *                    i.e. reading T2M0 latches T2M1 and T2MOVF0-T2MOVF2.
 *
 * @param       uint32* coarse - Coarse system time in 625us ticks.
 * @param       uint16* fine   - Fine system time in 31.25ns ticks.
 *
 * @return      void
 */
void halTimer2GetCapturedTime(uint16 *fine, uint32 *coarse) {
    // select T2 Timer and Overflow 24-bit counter
    T2MSEL = T2MSEL_T2MSEL_TIMER_CAPTURE | T2MSEL_T2MOVFSEL_OVERFLOW_CAPTURE;

    if(fine) {
        // Read Timer to latch T2M1 and T2MOVF0-T2MOVF2.
        ((uint8 *)fine)[0] = T2M0;
        ((uint8 *)fine)[1] = T2M1;
    }

    if(coarse) {
        // Read overflow counter.
        ((uint8 *)coarse)[0] = T2MOVF0;
        ((uint8 *)coarse)[1] = T2MOVF1;
        ((uint8 *)coarse)[2] = T2MOVF2;
        ((uint8 *)coarse)[3] = 0;
    }
  return;
}


/*******************************************************************************
* @fn           halTimer2Reset
*
* @brief        Reset Timer2 and clear counters and interrupts.
*
* @param        void
*
* @return       void
*/
void halTimer2Reset(void) {
    // Stop timer 2.
    T2CTRL = 0x00;
    // Modify timer count value.
    T2MSEL = 0x00;
    // Set timer 2 count value.
    T2M0 = 0x00;
    T2M1 = 0x00;
    // Set timer 2 overflow count registers to zero.
    T2MOVF0 = 0x00;
    T2MOVF1 = 0x00;
    T2MOVF2 = 0x00;
    // Clear All Timer2 Interrupt flags.
    T2IRQF = 0;
    // Clear [IRCON.T2IF] ( Timer 2 interrupt flag ).
    T2IF = 0;
    return;
}


/*******************************************************************************
* @fn           halTimer2SetBasePeriod
*
* @brief        Set the base period for the overflow comapre mode.
*
* @param        uint16 nu_of_31_25ns_ticks: Number of 31.25 ns ticks.
*
* @return       void
*/
void halTimer2SetBasePeriod(uint16 nu_of_31_25ns_ticks) {
    // Modify timer 2 period.
    T2MSEL = 0x02;
    // Set Basic period.
    T2M0 = (uint8) nu_of_31_25ns_ticks;
    T2M1 = (uint8)(nu_of_31_25ns_ticks >> 8);
    return;
}


/*******************************************************************************
* @fn           halTimer2SetOverflowPeriod
*
* @brief        Update Timer2 overflow period.
*
* @param        unsigned long overFlowPeriod: Overflow period.
*
* @return       void
*/
void halTimer2SetOverflowPeriod(uint32 overFlowPeriod) {
    // Modify timer 2 overflow period.
    T2MSEL = 0x20;
    // Update overflow compare registers.
    T2MOVF0 = (uint8) (overFlowPeriod & 0xFF);
    T2MOVF1 = (uint8) ((overFlowPeriod >> 8) & 0xFF);
    T2MOVF2 = (uint8) ((overFlowPeriod >> 16) & 0xFF);
    return;
}


/*******************************************************************************
* @fn           halTimer2SetTimerCount
*
* @brief        Set Timer2 count.
*
* @param        unsigned long overFlowPeriod: Overflow count
*
* @param        unsigned short basePeriod: Base count.
*
* @return       void
*/
void halTimer2SetTimerCount(uint16 baseCount, uint32 overFlowCount) {
    uint8 IntState;
    HAL_INT_LOCK(IntState);  // Start Critical section.

    // Modify timer 2 count value.
    T2MSEL = 0x00;

    // Set timer 2 count value.
    T2M0 = (uint8) (baseCount & 0x00FF);
    T2M1 = (uint8) ((baseCount >> 8) & 0x00FF);

    // Update overflow compare registers.
    T2MOVF0 = (uint8) (overFlowCount & 0xFF);
    T2MOVF1 = (uint8) ((overFlowCount >> 8) & 0xFF);
    T2MOVF2 = (uint8) ((overFlowCount >> 16) & 0xFF);

    HAL_INT_UNLOCK(IntState); // End Critical section.
}


/*******************************************************************************
* @fn           halTimer2UpdateTimerCount
*
* @brief        Update Timer2 count. Add values in argument to current value.
*
* @param        unsigned short baseTicks: Base count.
* @param        unsigned long ovfTicks: Overflow count
* @param        unsigned short basePeriod: Base period.
* @param        unsigned short ovfPeriod: Overflow period.
*
* @return       void
*/
void halTimer2UpdateTimerCount(uint16 baseTicks, uint32 ovfTicks, uint16 basePeriod, uint32 ovfPeriod) {
    uint8 IntState;
    HAL_INT_LOCK(IntState);

    uint16 fine;
    uint32 coarse;

    // Read Current timer value.
    halTimer2GetFullCurrentTime(&fine, &coarse);

    // Add current value with update.
    fine += baseTicks;
    coarse += ovfTicks;

    // Check for base overflow.
    if(basePeriod) {
      while(fine >= basePeriod) {
            fine -= basePeriod;
            coarse++;
            // Redo check until no more overflow occurs.
        }
    }

    // Check for ovf overflow.
    if(ovfPeriod) {
      while(coarse >= ovfPeriod) {
            // Overflow of ovf period has occured -> Wrap around zero.
            coarse -= ovfPeriod;
            // Redo check until no more overflow occurs.
        }
    }
    else if(coarse & 0xFF000000) {
        // Overflow of 3 byte ovf value has occured -> Wrap around zero.
        coarse &= ~0xFF000000;
    }

    // Update timer2 count.
    halTimer2SetTimerCount(fine, coarse);
    HAL_INT_UNLOCK(IntState);
    return;
}


/*******************************************************************************
* @fn           halTimer2Start
*
* @brief        Starts Timer2 counter.
*
* @param        uint8 sync: True if timer 2 should use synchronized start.
*               When synch is activated timer 2 will wait for 32-kHz clock
*               rising edge and then automatically calculate and update timer2
*               with the time passed sinces the last synchronized timer2 stop.
*               The time passed is calculated from the number of 32 kHz ticks
*               and conveted to 31.25 ns timer 2 ticks.
*
*               Only avaiable for CC2541 and CC2545 and requires an external
*               32 kHz XTAL on HW module.
*
* @return       void
*/
void halTimer2Start(uint8 sync) {
    uint8 IntState;
    HAL_INT_LOCK(IntState);         // Start critical section.
#if((chip==2541) || (chip==2545))
  if(sync) {
        T2CTRL |= T2CTRL_SYNC;
  }
#else
    // For CC2543 always write 0 to SYNC when using timer2.
    T2CTRL &= ~T2CTRL_SYNC;
#endif
    // start the Timer 2 ticking.
    T2CTRL |= T2CTRL_RUN;
    // Note: Assumes the hardware will work and the bit will change!
    while( !(T2CTRL & T2CTRL_STATE) );
    HAL_INT_UNLOCK(IntState);       // End critical section.
    return;
}


/*******************************************************************************
* @fn           halTimer2Stop
*
* @brief        Starts Timer2 counter.
*
* @param        uint8 sync: True if timer 2 should use synchronized stop.
*               When synch is activated timer 2 will stop at a 32 kHz rising
*               edge and will then read and store the current sleep timer clock
*               value.
*
* @return       Void
*/
void halTimer2Stop(uint8 sync) {
    uint8 IntState;
    HAL_INT_LOCK(IntState);
#if((chip==2541) || (chip==2545))
  if(sync) {
        T2CTRL |= T2CTRL_SYNC;
  }
#else
    // For CC2543 always write 0 to SYNC when using timer2.
    T2CTRL &= ~T2CTRL_SYNC;
#endif
    // Stop Timer 2, and initialize synch with 32KHz and latch mode.
    T2CTRL &= ~T2CTRL_RUN;
    // Note: Assumes the hardware will work and the bit will change!
    while( (T2CTRL & T2CTRL_STATE) );
    HAL_INT_UNLOCK(IntState);
    return;
}



/*******************************************************************************
* @fn           halTimer2UpdateOverflowCompareValue
*
* @brief        Update Timer2 overflow compare value.
*               32 MHz XTAL must be running for correct operation.
*
* @param        uint8 intervalStepSize625us:
*               Number of 625 us steps (range 0 to 10.24 ) (min 160 to max 16384).
*               Number of 78.125 us steps (range 0 to 10.24 ) (min 0 to max 128).
*
* @param        unsigned char extraDelayStepSize78125ns:
*               0-128 extra periods. Most significant bit will be cleared.
*
*
* @return       void
*/
void halTimer2UpdateOverflowCompareValue(unsigned long intervalStepSize625us, unsigned char extraDelayStepSize78125ns)
{
    unsigned char IntState;

    HAL_INT_LOCK(IntState);

    /* Add interval and extra delay. */

    // Multiply intervalStepSize625us by 8 to convert to 625 us units.
    unsigned long overflowCount = (intervalStepSize625us << 3);
    overflowCount += extraDelayStepSize78125ns;

    // Modify timer 2 overflow period.
    T2MSEL = 0x20;

    // Update overflow compare registers.
    T2MOVF0 = (unsigned char) (overflowCount & 0x00FF);
    T2MOVF1 = (unsigned char) ((overflowCount >> 8) & 0x00FF);
    T2MOVF2 = (unsigned char) ((overflowCount >> 16) & 0x00FF);

    HAL_INT_UNLOCK(IntState);
}