/******************************************************************************
*   Filename:       miniBLE_scheduler.h
*   Revised:        $Date: 2014-06-26 12:57:28 +0200 (to, 26 jun 2014) $
*   Revision:       $Revision: 122 $
*
*   Description:    Header file for miniBLE Scheduler.
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

/*  Below is some simple illustrations explaining the BLE advertisment scheme.
     -------------------           -------------------           -------------------
    |  ADV_NONCONN_IND  |         |  ADV_NONCONN_IND  |         |  ADV_NONCONN_IND  |
    |    Channel 37     |         |    Channel 38     |         |    Channel 39     |
    |     2402 MHz      |         |     2426 MHz      |         |      2480 MHz     |
     -------------------           -------------------           -------------------
    - max pcktInterval < 10 ms -> | --------------------------> |
   ^                                                                                ^
   Adv event start                                                               Stop

     ---------------             ---------------                ---------------
    |  Advertising  |           |  Advertising  |               |  Advertising  |
    |    Event      |           |    Event      |               |    Event      |
     ---------------             ---------------                ---------------
    ---- advInterval ----> |    ---- advInterval ----> |
    - advInterval + advDelay -> |- advInterval + advDelay ----> |
   ^
   Advertising state entered
*/

#ifndef MINIBLE_SCHEDULER_H
#define MINIBLE_SCHEDULER_H
/*******************************************************************************
* INCLUDES
*/
#include "hal_types.h"
#include "hal_defs.h"
#include "miniBLE_defs.h"



/*******************************************************************************
 * MACROS
 */
#define MINIBLE_SCHEDULER_SET_WAKEUP(time)                          \
st(                                                                 \
    globalWakeupTime = time;                                        \
    ST2 = (uint8)((time >> 16) & 0xFF);                             \
    ST1 = (uint8)((time >> 8) & 0xFF);                              \
    while( !(STLOAD & STLOAD_LDRDY) );                              \
    ST0 = (uint8)(time & 0xFF);                                     \
)

#define MINIBLE_SCHEDULER_READ_TIMER(time)                          \
st(                                                                 \
    while(!(SLEEPSTA & SLEEPSTA_CLK32K));                           \
    time = (uint32) ST0;                                            \
    time |= ((uint32) ST1) << 8;                                    \
    time |= ((uint32) ST2) << 16;                                   \
)

/*  Macro below reads timer immediately withtou waiting for updated value.
*   Can save from 0-30.5 us of execution time, but might offset timing with
*   the same duration. */
#define MINIBLE_SCHEDULER_READ_TIMER_INSTANT(time)                  \
st(                                                                 \
    time = ST0;                                                     \
    time |= ((uint32) ST1) << 8;                                    \
    time |= ((uint32) ST2) << 16;                                   \
)

// Set sleep mode.
#define MINIBLE_SCHEDULER_SET_SLEEP_MODE(powerMode)                    \
        SLEEPCMD &= ~0x03;                                          \
        SLEEPCMD |= powerMode;

/* Sleep timer CPU interrupt redefined as scheduler interrupt ([IRCON.STIF]) */
#define SCHEDULER_INT_TRIGGER()   (STIF = 1)
#define SCHEDULER_INT_CLEAR()     (STIF = 0)
#define SCHEDULER_INT_ENABLE()    (STIE = 1)
#define SCHEDULER_INT_DISABLE()   (STIE = 0)
#ifndef DELTA
#define DELTA(n,m)                                                  \
    (MAX(n,m) - MIN(n,m))
#endif
#define MINIBLE_SCHEDULER_PSEUDO_INIT(seed)                         \
st(                                                                 \
    RNDL = (uint8)(seed >> 8);                                      \
    RNDL = (uint8)seed;                                             \
)

//#define MINIBLE_SCHEDULER_PSEUDO_BYTE()     0
//#define MINIBLE_DELAY_MARGIN                0

#define MINIBLE_SCHEDULER_PSEUDO_BYTE()     RFPSRND
#define MINIBLE_DELAY_MARGIN                38  // 1.1602 m for up to 500 ms interval.

//#define MINIBLE_SCHEDULER_PSEUDO_BYTE()     (RFPSRND>>2)
//#define MINIBLE_DELAY_MARGIN                66  // 2.0151 ms for 1 second interval.


/*******************************************************************************
* DEFINES
*/

/*  The BLUETOOTH SPECIFICATION Version 4.1 requires a pseudorandom delay
*   between each advertisment event. So this is enabled by default. */
#define ADD_PESUDORANDOM_DELAY      1
#define NO_PESUDORANDOM_DELAY       0


/*  Margin used in scheduling to avoid missing events. The execution time of
*   the scheduler (ST_ISR) must be less than this margin to avoid missing
*   events. The reason why this is a problem is that the sleep timer compare
*   value must be set to more than 5 32-kHz-ticks than the current
*   sleep timer count or else the timer will miss the compare event and wrap
*   around (24 bit - 8,5 minutes).
*/
#if(POWER_SAVING)
#define ST_SCHEDULE_MARGIN 20   // 610 us.
#else
#define ST_SCHEDULE_MARGIN 15   // (305 + 152.5) us.
#endif


/*******************************************************************************
* GLOBAL VARIABLES
*/
extern volatile uint32 globalWakeupTime;

/* Flags for keeping track of program flow in miniBLE. */
extern volatile uint8 abortSleepFlag;
extern volatile uint8 waitCompleteFlag;
extern volatile uint8 SchedulerEnabledFlag;

/*******************************************************************************
* GLOBAL FUNCTIONS
*/
void miniBleSchedulerReset(void);
uint8 miniBleSchedulerStart(void);
uint32 miniBleSchedulerGetTimeUntilNextEvent(void);
uint32 miniBleSchedulerGetTimeSincepreviousEvent(void);
void miniBleSchedulerScheduleTask(uint8 taskId);
uint8 miniBleSchedulerEnterSleep(void);
#endif