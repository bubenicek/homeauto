/*******************************************************************************
*  Filename:        hal_timer2.h
*  Revised:         $Date: 2014-06-26 16:14:11 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13250 $
*
*  Description:     HAL for radio timer Timer 2.
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


#ifndef _HAL_TIMER2_H
#define _HAL_TIMER2_H

/*******************************************************************************
* INCLUDES
*/
#include "hal_types.h"



/*******************************************************************************
 * MACROS
 */
#define HAL_TIMER2_DISABLE_EVENTS()        T2EVTCFG = 0x77;                        // No timer 2 events sent to LLE.



/******************************************************************************
* DEFINES
*/
// Defines for equivalent time number corresponding to number of 31.25 ns ticks.
#define TICK_VALUE_1US          32
#define TICK_VALUE_10US         320
#define TICK_VALUE_100US        3200
#define TICK_VALUE_1MS          32000

#define VALUE_1MS_IN_1US_UNITS  1000
#define VALUE_10MS_IN_1US_UNITS 10000

#define PERIODIC    1
#define SINGLE      0

// T2IRQF (0xA1) – Timer 2 Interrupt Flags
#define TIMER2_LONG_COMPARE2F   0x80
#define TIMER2_LONG_COMPARE1F   0x40
#define TIMER2_OVF_COMPARE2F    0x20
#define TIMER2_OVF_COMPARE1F    0x10
#define TIMER2_OVF_PERF         0x08
#define TIMER2_COMPARE2F        0x04
#define TIMER2_COMPARE1F        0x02
#define TIMER2_PERF             0x01

// T2IRQM (0xA7) – Timer 2 Interrupt Mask
#define TIMER2_LONG_COMARE2M    0x80
#define TIMER2_LONG_COMARE1M    0x40
#define TIMER2_OVF_COMPARE2M    0x20
#define TIMER2_OVF_COMPARE1M    0x10
#define TIMER2_OVF_PERM         0x08
#define TIMER2_COMPARE2M        0x04
#define TIMER2_COMPARE1M        0x02
#define TIMER2_PERM             0x01

// T2MSEL (0xC3) – Timer 2 Multiplex Select
#define T2MSEL_T2MOVFSEL                            0x70
    #define T2MSEL_T2MOVFSEL_OVERFLOW_COUNTER       (0x00<<4)
    #define T2MSEL_T2MOVFSEL_OVERFLOW_CAPTURE       (0x01<<4)
    #define T2MSEL_T2MOVFSEL_OVERFLOW_PERIOD        (0x02<<4)
    #define T2MSEL_T2MOVFSEL_OVERFLOW_COMPARE1      (0x03<<4)
    #define T2MSEL_T2MOVFSEL_OVERFLOW_COMPARE2      (0x04<<4)
#define T2MSEL_T2MSEL                               0x07
    #define T2MSEL_T2MSEL_TIMER_COUNT_VALUE         0x00
    #define T2MSEL_T2MSEL_TIMER_CAPTURE             0x01
    #define T2MSEL_T2MSEL_TIMER_PERIOD              0x02
    #define T2MSEL_T2MSEL_TIMER_COMPARE1            0x03
    #define T2MSEL_T2MSEL_TIMER_COMPARE2            0x04

// T2IRQF (0xA1) – Timer 2 Interrupt Flags
#define T2IRQF_LONG_COMPARE2F   0x80
#define T2IRQF_LONG_COMPARE1F   0x40
#define T2IRQF_OVF_COMPARE2F    0x20
#define T2IRQF_OVF_COMPARE1F    0x10
#define T2IRQF_OVF_PERF         0x08
#define T2IRQF_COMPARE2F        0x04
#define T2IRQF_COMPARE1F        0x02
#define T2IRQF_PERF             0x01

// T2IRQM (0xA7) – Timer 2 Interrupt Mask
#define T2IRQM_LONG_COMARE2M    0x80
#define T2IRQM_LONG_COMARE1M    0x40
#define T2IRQM_OVF_COMPARE2M    0x20
#define T2IRQM_OVF_COMPARE1M    0x10
#define T2IRQM_OVF_PERM         0x08
#define T2IRQM_COMPARE2M        0x04
#define T2IRQM_COMPARE1M        0x02
#define T2IRQM_PERM             0x01


/*******************************************************************************
* PROTOTYPE FUNCTIONS
*/
void halTimer2Reset(void);
void halTimer2Start(uint8 sync);
void halTimer2Stop(uint8 sync);
void halTimer2EnableInterrupt(uint8 t2irqmSelection);
void halTimer2DisableInterrupt(uint8 t2irqmSelection);
void halTimer2GetFullCurrentTime(uint16 *fine, uint32 *coarse);
void halTimer2GetCapturedTime(uint16 *fine, uint32 *coarse);
void halTimer2SetOverflowPeriod(uint32 overFlowPeriod);
void halTimer2SetTimerCount(uint16 baseCount, uint32 overFlowCount);
void halTimer2UpdateTimerCount(uint16 baseTicks, uint32 ovfTicks, uint16 basePeriod, uint32 ovfPeriod);
void halTimer2SetBasePeriod(uint16 nu_of_31_25ns_ticks);
void halTimer2SetRadioTimeout(uint32 ms, uint8 periodic);
void halTimer2UpdateOverflowCompareValue(unsigned long intervalStepSize625ns, unsigned char extraDelayStepSize78125ns);

#endif