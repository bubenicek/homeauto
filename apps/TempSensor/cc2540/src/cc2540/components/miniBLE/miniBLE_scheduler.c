/******************************************************************************
*   Filename:       miniBLE_scheduler.c
*   Revised:        $Date: 2014-06-26 12:57:28 +0200 (to, 26 jun 2014) $
*   Revision:       $Revision: 122 $
*
*   Description:    miniBLE Scheduler.
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


/******************************************************************************
* INCLUDES
*/
#if(chip==2543)
#include "ioCC2543.h"
#else
#warning "Device is not supported!"
#endif
#include "miniBLE_scheduler.h"
#include "miniBLE_phy.h"
#include "miniBLE_defs.h"
#include "miniBLE.h"
#include "prop_regs.h"
#include "ioCC254x_bitdef.h"
#include "hal_mcu.h"
#include "hal_types.h"
#include "hal_int.h"
#include "clock.h"
#include "power_mode.h"
#if(SMARTRF05EB)
#include "hal_assert.h"
#endif


/******************************************************************************
* GLOBAL VARIABLES
*/
/* Global variables for scheduling compare events in the sleep timer (ST) module. */
volatile uint32 globalWakeupTime    = 0;
volatile uint32 globalWakeupTimeOld = 0;
volatile uint8 waitCompleteFlag     = 0;
volatile uint8 SchedulerEnabledFlag = 0;
volatile uint8 triggeredTaskEvents  = 0;
uint8 intervalCount, calibrationFlag = 0;

/*  If we need an delay between the transmitted packets on the different active
*   broadcast channels we need an additional trigger variable. */
#if(TX_INTERVAL)
uint8 lastChannelFlag = 0;
#endif


/*******************************************************************************
* GLOBAL FUNCTIONS
*/
/*******************************************************************************
* @fn           sleep_isr
*
* @brief        Interrupt service routine for the sleep timer. This is the only
*               timing reference used in miniBLE. The isr is formed as a state
*               machine which continously schedules the next event to occur:
*               advertisement (ADV) or data aquisition/processing (DAQ).
*
* @param        void
*
* @return       void
*/
HAL_ISR_FUNCTION_SPEED_OPTIMIZE(SLEEP_ISR, ST_VECTOR) {
    uint8 intState, bufferByte, i;

    //P1_0 = 1;

    uint8 deltaTrigger = 0;
    uint32 bufferLong, bufferLong2, deltaTime, timeNow;
    HAL_INT_LOCK(intState);     // Enter Critical section.
    SCHEDULER_INT_CLEAR();      // Clear Sleep Timer CPU interrupt flag.


    /******************** Update task counters ****************************/
    MINIBLE_SCHEDULER_READ_TIMER(timeNow);
    if(timeNow > globalWakeupTimeOld) {
        deltaTime = timeNow;
        deltaTime -= globalWakeupTimeOld;
    } else if (timeNow < globalWakeupTimeOld) {
        // Sleep Timer (ST) has wrapped around (24 bit).
        deltaTime = (0x00FFFFFF - globalWakeupTimeOld);
        // The jump from 0x00FFFFFF to 0 accounts for + 1.
        deltaTime += timeNow + 1;
    } else {
        // ST is manually triggered.
        deltaTime = 0;
    }

    /*  Calculate the delta/difference in time between globalWakeupTime and
    *   timeNow to adjust for this when subtracting the count down values. */
    if(timeNow >= globalWakeupTime) {
        deltaTrigger = (uint8) timeNow - globalWakeupTime;
    } else {
        deltaTrigger = (uint8) timeNow;
        deltaTrigger += (uint8) ((0x00FFFFFF - globalWakeupTimeOld) + 1);
    }

    // Update old trigger ref. to now instead of globalWakeupTime.
    globalWakeupTimeOld = timeNow;

    /* Update task countdown values and set flags. */
    for(i=0; i < NUM_TASKS; i++) {
        if(miniBleTask[i].enable) {
            // deltaTime will never overflow as it is used as 24 bit variable.
            if( (deltaTime + ST_SCHEDULE_MARGIN) < miniBleTask[i].countdown) {
                miniBleTask[i].countdown -= deltaTime;
            } else {
                miniBleTask[i].flag = 1;
                /*  Reset countdown interval and adjust delta offset to make sure
                *   the next event is relative to last ADV globalWakeupTime trigger. */
                miniBleTask[i].countdown = miniBleTask[i].interval - deltaTrigger;
            }
        }
    }

    /******************** SCHEDULER PART *********************************/
    if(miniBleTask[TASK_ADV].flag) {
#if(TX_INTERVAL)
        if(nuOfActiveChannels > 1) {
            if(miniBleAdvEventDone) {
                // First packet in event.
                intervalCount = nuOfActiveChannels;
                miniBleAdvEventDone = 0;
                /*  Indicate that the first ADV task has been triggered
                *   meaning the start of the advertisment event. */
                triggeredTaskEvents |= BV(TASK_ADV);
            }
            if(--intervalCount) {
                // Set default interval (9 ms) between packets when TX_INTERVAL is enabled.
                miniBleTask[TASK_ADV].countdown = PACKET_INTERVAL - deltaTrigger;
            } else {
                /*  This is the last packet in this advertisment event. restore
                *   the adv task count down value relative to the start of the event. */
                bufferLong = PACKET_INTERVAL * (nuOfActiveChannels - 1 );
                bufferLong2 = miniBleTask[TASK_ADV].interval - bufferLong;
                miniBleTask[TASK_ADV].countdown = bufferLong2 - deltaTrigger;
#if(PSEUDO_ADV_DELAY)
                bufferByte = MINIBLE_SCHEDULER_PSEUDO_BYTE();
                miniBleTask[TASK_ADV].countdown += (bufferByte + MINIBLE_DELAY_MARGIN);
#endif
            }
        } else {
            // Only one active channel, no need for the logic above.
        }
#else
#if(PSEUDO_ADV_DELAY)
        bufferByte = MINIBLE_SCHEDULER_PSEUDO_BYTE();
        miniBleTask[TASK_ADV].countdown += (bufferByte + MINIBLE_DELAY_MARGIN);
#endif
#endif
    }

    // If wait task is triggered, disable task (one shot after task is enabled).
    if(miniBleTask[TASK_WAIT].flag) {
        miniBleTask[TASK_WAIT].enable = 0;
    }

    /*  Schedule next event: */
    uint32 smallestCountdown = 0x00FFFFFF;
    bufferLong = miniBleTask[TASK_ADV].countdown;
    for(i=0; i < NUM_TASKS; i++) {
        if(miniBleTask[i].enable) {
            bufferLong2 = miniBleTask[i].countdown;
            if(smallestCountdown > bufferLong2) {
                if(miniBleTask[TASK_ADV].enable) {
                    // If event is to close to next TASK_ADV event, sync it with adv.
                    if( DELTA(bufferLong2, bufferLong) < ST_SCHEDULE_MARGIN ) {
                        miniBleTask[i].countdown = bufferLong;
                    }
                }
                smallestCountdown = miniBleTask[i].countdown;
            }
        }
    }
    if(smallestCountdown >= 0x00FFFFFF) {
        /*  No active tasks or illegal interval.
        *   Should never occur! */
#if(SMARTRF05EB)
        halAssertHandler();
#else
        while(1);
#endif
    } else {
        /*  Set new wakeup time bsed on time read at the beginning of this
        *   ISR + the next task countdown (smallest count down). */
        MINIBLE_SCHEDULER_SET_WAKEUP(timeNow + smallestCountdown);
    }


    /************************ Perform Tasks **********************************/

    /*  The variable "event" is used to indidcate state to the upper layers.
    *   If both ADV and DAQ happens at the same time, then event will be set
    *   to DAQ as this is needed by the application layer, while the ADV state
    *   is automatically handled in the lower layers. */

    /* Perform Pending tasks, ADV has highest priority */
    if(miniBleTask[TASK_ADV].flag && miniBleTask[TASK_ADV].enable) {
        miniBleTask[TASK_ADV].flag = 0;     // Clear task flag.
        miniBleAdvTxDone = 0;               // Clear TxDone flag.
        /*  Wakeup event is for start of broadcast event. Check for stable clock
        *   and start transmission on first active broadcast channel. */

        /*  Wait until 32 MHz XTAL is stable. Dont need to select 32 MHz XTAL
        *   as clock source as this should be ebaled at all times after initialization. */
        CC254x_WAIT_CLK_UPDATE();
        //P1_0 = 0;
        MINIBLE_PHY_CMD(CMD_TX);

#if(!TX_INTERVAL)
        triggeredTaskEvents |= BV(TASK_ADV);
#endif
    }


    /* TASK_DAQ. */
    if(miniBleTask[TASK_DAQ].flag && miniBleTask[TASK_DAQ].enable) {
        miniBleTask[TASK_DAQ].flag = 0;     // Clear task flag.
        triggeredTaskEvents |= BV(TASK_DAQ);
    }

    /*  If calibration was enabled at last wakeup, then disable it now.
    *   After coming up from power mode 1-3 the system will initally run on the
    *   16MHz RCOSC and then change to the 32 MHz XTAL and then calibration of
    *   the 32 kHz RCOSC will start and last for about 2 ms. */
    if(calibrationFlag) {
        CC254x_WAIT_CLK_UPDATE();
        CC254x_DISABLE_32KHZ_CALIBRATION();
        calibrationFlag = 0;
    }

    /*  CALIBRATION_TASK ( 32 kHz RCOSC Calibration ).
    *   This implementation is inefficient as it prohibit any other code to run
    *   during calibration (Stuck in this ISR for 3.5 ms). The reason for placing
    *   the routine here was to make it simpler for the user as nothing has to be
    *   implented in main context from the application side. */
    if(miniBleTask[TASK_CAL].flag && miniBleTask[TASK_CAL].enable) {
        miniBleTask[TASK_CAL].flag = 0;     // Clear task flag.

#if(!POWER_SAVING)
        /*  If a radio operation is in progress (TX-broadcast) then wait until
        *   all current radio opearations are complete because the 32 MHz XTAL
        *   must be running during all RF activities. */
        if(!miniBleAdvTxDone) {
#if(!TX_INTERVAL)
            /*  Exit Critical section if back2back transmissions is epxected
            *   which will be initiated in the RF_ISR. */
            HAL_INT_UNLOCK(intState);
#endif
            MINIBLE_PHY_WAIT_FOR_TASKDONE();
        }

        /*  If an advertisement task (TASK_ADV) is due within the next 4 ms
        *   delay the calibration until then. */
        if( miniBleTask[TASK_ADV].countdown <= INTERVAL_4_mSEC) {
            miniBleTask[TASK_CAL].countdown = miniBleTask[TASK_ADV].countdown;
        } else {
            CC254x_START_32KHZRC_CALIBRATION(XTAL_GUARD_TIME);
        }
#else

        // Enable bit to indicate to sleep function to enable the calibration bit in sleepcmd
        CC254x_ENABLE_32KHZ_CALIBRATION();
        calibrationFlag = 1;

        // Then disable the bit the next time .... Can just change the register here......
        // Jst disable calibration every time we wake up.,.....

#endif
        triggeredTaskEvents |= BV(TASK_CAL);
    }

    /* TASK_WAIT. */
    if(miniBleTask[TASK_WAIT].flag) {
        miniBleTask[TASK_WAIT].flag = 0;
        waitCompleteFlag = 1;
        triggeredTaskEvents |= BV(TASK_WAIT);
    }

    EnterSleepModeFlag = 0;         // Set flag to stop device from going into sleep.
    HAL_INT_UNLOCK(intState);       // Exit Critical section.
}


/******************************************************************************
*****************************  WARNING  ***************************************
The functions below write to variables used by the miniBLE scheduler (ST ISR).*
Use of these functions might cause the scheduler to crash and should only be  *
used by the miniBLE layer.                                                    *
******************************************************************************/


/******************************************************************************
* @fn           miniBleSchedulerStart
*
* @brief        Start scheduler. Either TASK_DAQ or TASK_ADV must be enabled
*               for the scheduler to start successfully.
*
* @param        void
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleSchedulerStart(void) {
    uint8 i;
    uint32 smallestCountdown = 0x00FFFFFF;

    /* Find the first event to be scheduled */
    for(i=0; i < NUM_TASKS; i++) {
        if(miniBleTask[i].enable) {
            // If at least one task is active, then scheduler is running.
            SchedulerEnabledFlag = 1;
            if(smallestCountdown > miniBleTask[i].countdown) {
                smallestCountdown = miniBleTask[i].countdown;
            }
        }
    }
    if(!SchedulerEnabledFlag) {
        // If no events are enabled, return failed status.
        return MINIBLE_FAIL_ALL_EVT_DISABLED;
    }

    // If an advertisment event is ongoing, wait until done (max approx 30 ms).
    while(!miniBleAdvEventDone);

    /*  Disable global interrupt. This will ensure that the scheduler starts
    *   correctly if the scheduler is already running. */
    HAL_INT_OFF();

    /*  Read and store current count of sleep timer into global variable and set
    *   initial trigger time for first event to be scheduled. */
    uint32 sleepTimerCount = 0;
    MINIBLE_SCHEDULER_READ_TIMER(sleepTimerCount);
    globalWakeupTime = sleepTimerCount;  // Init Current global time stamp...

    // smallestCountdown must be at least 5 more then the current sleep timer count.
    MINIBLE_SCHEDULER_SET_WAKEUP(sleepTimerCount + smallestCountdown);

    // Clear [IRCON.STIF] (Sleep Timer CPU interrupt flag).
    SCHEDULER_INT_CLEAR();
    // Set the individual, interrupt enable bit [IEN0.STIE=1].
    SCHEDULER_INT_ENABLE();
    // Enable global interrupt.
    HAL_INT_ON();
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn           miniBleSchedulerReset
*
* @brief        Stop the scheduler and reset all dependant variables. After this
*               function is run miniBleSchedulerStart() can be called to start
*               the scheduler successfully again
*               ( holds true if (advEnabled | daqEnabled) != 0 )
*
* @param        void
*
* @return       void
*/
void miniBleSchedulerReset(void) {
    uint8 intState;

    // If an advertisment event is ongoing, wait until done (max approx 30 ms).
    while(!miniBleAdvEventDone);
    HAL_INT_LOCK(intState);     // Enter Critical section.

    // Clear flag indicating status of scheduler.
    SchedulerEnabledFlag = 0;

    /*  Clear the individual, interrupt enable bit [IEN0.STIE=0].
    *   This will halt the scheduler as it is dependant on the ST ISR. */
    SCHEDULER_INT_DISABLE();
    // Clear [IRCON.STIF] (Sleep Timer CPU interrupt flag).
    SCHEDULER_INT_CLEAR();

    // Reset all countdown values for all tasks.
    for(uint8 i=0; i < NUM_TASKS; i++) {
        miniBleTask[i].countdown = miniBleTask[i].interval;
    }
    HAL_INT_UNLOCK(intState);   // Exit Critical section.
    return;
}


/******************************************************************************
* @fn           miniBleSchedulerScheduleTask
*
* @brief        Update selected task timings parameters and trigger scheduler to
*               allow the task to be seamlessly added to the schedule.
*
* @param        taskId The selected task.
*
* @return       void
*/
void miniBleSchedulerScheduleTask(uint8 taskId) {
    uint32 sleepTimerValue;
    miniBleTask[taskId].countdown = miniBleTask[taskId].interval;
    miniBleTask[taskId].countdown +=  miniBleSchedulerGetTimeSincepreviousEvent();
    // Update current trigger value to current time (right now).
    MINIBLE_SCHEDULER_READ_TIMER_INSTANT(sleepTimerValue);
    globalWakeupTime = sleepTimerValue;
    SCHEDULER_INT_TRIGGER();
    return;
}


/******************************************************************************
* @fn           miniBleSchedulerGetTimeUntilNextEvent
*
* @brief        Find the remaining time until next event.
*               Resolution: approx +/- 2 timer ticks
*
* @param        void
*
* @return       time until next event in 32 kHz timer ticks. If value returned
*               is 0 this means that the event has already occured.
*/
#pragma optimize=speed
uint32 miniBleSchedulerGetTimeUntilNextEvent(void) {
    uint8 intState;
    uint32 retVal;
    HAL_INT_LOCK(intState);         // Enter Critical section.
    uint32 currentSleepTimerValue;
    /*  Read the sleep timer value without waiting for ready flank
    *   (SLEEPSTA_CLK32K high in SLEEPSTA register). This might
    *   result in reading one less tick than actual. */
    MINIBLE_SCHEDULER_READ_TIMER_INSTANT(currentSleepTimerValue);
    if(STIF == 1) {
        /*  event has already been triggered and the ST isr will be entered
        *   immediately after global interrupt is enabled (EA = 1). */
        HAL_INT_UNLOCK(intState);   // Exit Critical section.
        return 0;
    }
    else if(currentSleepTimerValue > globalWakeupTime) {
        /*  If current count is larger than next compare value, this means the sleep
        *   timer has to wrap around before it will reach the next event. */
        retVal = (0x00FFFFFF - currentSleepTimerValue + globalWakeupTime + 1) - 1;
        HAL_INT_UNLOCK(intState);   // Exit Critical section.
    } else {
        retVal = globalWakeupTime - currentSleepTimerValue - 1;
        HAL_INT_UNLOCK(intState);   // Exit Critical section.
    }
    return retVal;
}


/******************************************************************************
* @fn           miniBleSchedulerGetTimeSincepreviousEvent
*
* @brief        Find the elapsed time since last event was triggered.
*
* @param        void
*
* @return       time since previous event in 32 kHz timer ticks.
*/
#pragma optimize=speed
uint32 miniBleSchedulerGetTimeSincepreviousEvent(void) {
    uint8 intState;
    uint32 retVal;
    HAL_INT_LOCK(intState);     // Enter Critical section.
    MINIBLE_SCHEDULER_READ_TIMER(retVal);
    if(retVal > globalWakeupTimeOld) {
        retVal -= globalWakeupTimeOld;
    } else if (retVal < globalWakeupTimeOld) {
        retVal += (0x00FFFFFF - globalWakeupTimeOld) + 1;
    } else {
        /*  This can only happen if ST is manually triggered which should
        *   not occur..... */
        retVal = 0;
    }
    HAL_INT_UNLOCK(intState); // Exit Critical section.
    return retVal;
}


/******************************************************************************
* @fn           miniBleSchedulerEnterSleep
*
* @brief        Enter sleep in miniBLE in a safe manner. If sleep is entered
*               directly without entering this function the scheduler may crash
*               and miniBLE execution may halt.
*
* @param        void
*
* @return       Events triggered.
*/
#pragma optimize=speed
uint8 miniBleSchedulerEnterSleep(void) {
    uint8 intState, powerMode = 0, retVal;
    uint32 timeLeft;
#if(POWER_SAVING)
    powerMode = 2;
#endif
    HAL_INT_LOCK(intState);     // Enter Critical section.
    // Find remaining time until next event.
    timeLeft = miniBleSchedulerGetTimeUntilNextEvent();
    EnterSleepModeFlag = 1;
    HAL_INT_UNLOCK(intState);   // Exit Critical section.

    /*  If the time until next event (ST trigger) is less than the required
    *   margin (300-600 us), do not enter sleep because the time it takes to
    *   disable and enable the 32 MHz clock will require more time than what is
    *   available. */

    // There might be time left.
    if(timeLeft){
        /*  If transmitter is active or if next event comes within 3 ms,
        *   do not stop 32 MHz XTAL, only halt CPU instead. */
#if(POWER_SAVING)
        if( !miniBleAdvTxDone || ( timeLeft < 100 ) ) {
            powerMode = 0;
        }
        if(powerMode > 0) {
            // Wait for the LLE to become idle before setting the LLE in reset.
            while(!miniBleAdvTxDone);
            while(!MINIBLE_PHY_IS_LLE_IDLE());
            // Set LLE in Reset to avoid unknown state on wake-up.
            MINIBLE_PHY_RESET_LLE();
        }
#endif
        /*  Set power mode, disable 32 kHz calibration and enter sleep if
        *   allowed (i.e. EnterSleepModeFlag not cleared by ST_ISR, RF_ISR
        *   or other..). */
        MINIBLE_SCHEDULER_SET_SLEEP_MODE(powerMode);
        EnterSleepModeDisableInterruptsOnWakeupUsingFlag();
        /*  Only interrupts can either wake up the device or clear the
        *   EnterSleepModeFlag, stopping the device from entering sleep,
        *   and hence "pass" to this point. */
        if(powerMode > 0) {
            // Enable the LLE (radio).
            MINIBLE_PHY_ENABLE_LLE();
            // If not 32 MHz might need to be enabled here....
        }
        /*  Global interrupt (EA) is disabled in the called function:
        *   EnterSleepModeDisableInterruptsOnWakeupUsingFlag, so it must be
        *   Enabled again. */
        HAL_INT_ON();
    }

    HAL_INT_LOCK(intState);         // Enter Critical section.
    retVal = triggeredTaskEvents;   // Copy triggered events.
    triggeredTaskEvents = 0;        // Clear triggered events.
    HAL_INT_UNLOCK(intState);       // Exit Critical section.
    return retVal;                  // Return Triggered events.
}