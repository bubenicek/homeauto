/*******************************************************************************
*  Filename:        hal_sleep.h
*  Revised:         $Date: 2014-02-06 16:28:25 +0100 (to, 06 feb 2014) $
*  Revision:        $Revision: 12078 $
*
*  Description:     Header file.
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
#ifndef _HAL_SLEEP_H
#define _HAL_SLEEP_H


/******************************************************************************
* DEFINES
*/
// Parameters for cc254X System clock-source select.
#define CLOCK_SRC_XOSC      0  // High speed Crystal Oscillator Control.
#define CLOCK_SRC_HFRC      1  // Low power RC Oscillator.

// for CC2545 external 32KHz crystal.
#define CLOCK_32K_XTAL      0  // 32.768 Hz crystal oscillator.
#define CLOCK_32K_RCOSC     1  // 32 kHz RC oscillator.

#define STOP                0
#define START               1

// Power modes
#define CPU_HALT                    0x00
#define POWER_MODE1                 0x01
#define POWER_MODE2                 0x02
#define POWER_MODE3                 0x03
#define DISABLE_RCOSC_CALIBRATION   0x80
#define ENABLE_RCOSC_CALIBRATION    0x00

// Bit masks to check CLKCON register.
#define CLKCON_OSC32K_BM    0x80  // bit mask, for the slow 32k clock oscillator.
#define CLKCON_OSC_BM       0x40  // bit mask, for the system clock oscillator.
#define CLKCON_TICKSPD_BM   0x38  // bit mask, for timer ticks output setting.
#define CLKCON_CLKSPD_BM    0x07  // bit maks, for the clock speed.

#define TICKSPD_DIV_1       (0x00 << 3)
#define TICKSPD_DIV_2       (0x01 << 3)
#define TICKSPD_DIV_4       (0x02 << 3)
#define TICKSPD_DIV_8       (0x03 << 3)
#define TICKSPD_DIV_16      (0x04 << 3)
#define TICKSPD_DIV_32      (0x05 << 3)
#define TICKSPD_DIV_64      (0x06 << 3)
#define TICKSPD_DIV_128     (0x07 << 3)

// Bit masks to check SLEEPSTA register.
#define SLEEP_XOSC_STB_BM   0x40  // bit mask, check the stability of XOSC.
#define SLEEP_HFRC_STB_BM   0x20  // bit maks, check the stability of the High-frequency RC oscillator.
#define SLEEP_OSC_PD_BM     0x04  // bit mask, power down system clock oscillator(s).


// IP1 (0xB9) – Interrupt Priority 0
#define IP1_ST_P0INT_WDT                0x20
#define IP1_ENC_T4_P1INT                0x10
#define IP1_T3_I2C                      0x08
#define IP1_URX0_T2_UTX0                0x04
#define IP1_ADC_T1_P2INT_USB            0x02
#define IP1_RFERR_RF_DMA                0x01


/*******************************************************************************
* MACROs
*/
/*
#if(chip==2543)
#define HAL_SLEEP_SET_POWER_MODE(powerMode)     SLEEPCMD = (SLEEPCMD & ~0x03) | powerMode;
#elif(chip==2541)
#define HAL_SLEEP_SET_POWER_MODE(powerMode)     SLEEPCMD = (SLEEPCMD & ~0x03) | powerMode;
#endif
*/
/*******************************************************************************
 * FUNCTIONS
 */
void halSleepEnterPowerMode(uint8 powerMode);
void halSleepSetSleepTimer(uint32 sleepDuration);
void halSleepSynchXOSCMsBase(uint8 start);
void halSleepEnableInterrupt(void);
void halSleepDisableInterrupt(void);

#endif