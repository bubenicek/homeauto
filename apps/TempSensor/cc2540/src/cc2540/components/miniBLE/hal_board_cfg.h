/******************************************************************************
*   Filename:       hal_board_cfg.h
*   Revised:        $Date: 2014-06-26 12:57:28 +0200 (to, 26 jun 2014) $
*   Revision:       $Revision: 122 $
*
*   Description:    Abstract board-specific registers, addresses,
*                   & initialization for H/W based on the CC254x (8051 core).
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
#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * INCLUDES
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"

/*******************************************************************************
 * CONSTANTS
 */

/* Board Identifier */
#if !defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_BOARD_CC2530EB_REV13)
  #define HAL_BOARD_CC2530EB_REV17
#endif

/* Clock Speed */

#define HAL_CPU_CLOCK_MHZ             32

/* Sleep Clock */

#define EXTERNAL_CRYSTAL_OSC          0x00  // external 32kHz XOSC
#define INTERNAL_RC_OSC               0x80  // internal 32kHz RCOSC

// For non-USB, assume external, unless an internal crystal is explicitly indicated.
#if !defined (XOSC32K_INSTALLED) || (defined (XOSC32K_INSTALLED) && (XOSC32K_INSTALLED == TRUE))
#define OSC_32KHZ                     EXTERNAL_CRYSTAL_OSC
#else
#define OSC_32KHZ                     INTERNAL_RC_OSC
#endif

// Minimum Time for Stable External 32kHz Clock (in ms)
#define MIN_TIME_TO_STABLE_32KHZ_XOSC 400

/* LCD Max Chars and Buffer */
#define HAL_LCD_MAX_CHARS   16
#define HAL_LCD_MAX_BUFF    25

/* LED Configuration */

#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)
  #define HAL_NUM_LEDS                 3
#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)
  #define HAL_NUM_LEDS                 1
#else
  #error Unknown Board Indentifier
#endif

#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x5800; i++) { }; } )

/* 1 - Green */
#define LED1_BV                        BV(0)
#define LED1_SBIT                      P1_0
#define LED1_DDR                       P1DIR
#define LED1_POLARITY                  ACTIVE_HIGH

#ifdef HAL_BOARD_CC2530EB_REV17
  /* 2 - Red */
  #define LED2_BV                      BV(1)
  #define LED2_SBIT                    P1_1
  #define LED2_DDR                     P1DIR
  #define LED2_POLARITY                ACTIVE_HIGH

  /* 3 - Yellow */
  #define LED3_BV                      BV(4)
  #define LED3_SBIT                    P1_4
  #define LED3_DDR                     P1DIR
  #define LED3_POLARITY                ACTIVE_HIGH
#endif

/* Push Button Configuration */

#define ACTIVE_LOW                     !
#define ACTIVE_HIGH                    !!    /* double negation forces result to be '1' */

/* S1 */
#define PUSH1_BV                       BV(1)
#define PUSH1_SBIT                     P0_1

#ifdef HAL_BOARD_CC2530EB_REV17
  #define PUSH1_POLARITY               ACTIVE_HIGH
#elif defined (HAL_BOARD_CC2530EB_REV13)
  #define PUSH1_POLARITY               ACTIVE_LOW
#else
  #error Unknown Board Indentifier
#endif

/* Joystick Center Press */
#define PUSH2_BV                       BV(0)
#define PUSH2_SBIT                     P2_0
#define PUSH2_POLARITY                 ACTIVE_HIGH

/* OSAL NV implemented by internal flash pages. */

// Flash is partitioned into 8 banks of 32 KB or 16 pages.
#define HAL_FLASH_PAGE_PER_BANK        16

// Flash is constructed of 32 pages of 1 KB.
#define HAL_FLASH_PAGE_PHYS            2048

// SNV can use a larger logical page size to accomodate more or bigger items or extend lifetime.
#define HAL_FLASH_PAGE_SIZE            HAL_FLASH_PAGE_PHYS
#define HAL_FLASH_WORD_SIZE            4

// CODE banks get mapped into the XDATA range 8000-FFFF.
#define HAL_FLASH_PAGE_MAP             0x8000

// The last 16 bytes of the last available page are reserved for flash lock bits.
#define HAL_FLASH_LOCK_BITS            16

// NV page definitions must coincide with segment declaration in project *.xcl file.
#if defined NON_BANKED
#define HAL_NV_PAGE_END                30
#elif defined HAL_BOARD_CC2541S
#define HAL_NV_PAGE_END                125
#elif defined HAL_BOARD_F128
#define HAL_NV_PAGE_END                62
#else
#define HAL_NV_PAGE_END                126
#endif

// Re-defining Z_EXTADDR_LEN here so as not to include a Z-Stack .h file.
#define HAL_FLASH_IEEE_SIZE            8
#define HAL_FLASH_IEEE_PAGE            (HAL_NV_PAGE_END+1)
#define HAL_FLASH_IEEE_OSET            (HAL_FLASH_PAGE_SIZE - HAL_FLASH_LOCK_BITS - HAL_FLASH_IEEE_SIZE)
#define HAL_INFOP_IEEE_OSET            0xC

#define HAL_NV_PAGE_CNT                2
#define HAL_NV_PAGE_BEG                (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)

// Used by DMA macros to shift 1 to create a mask for DMA registers.
#define HAL_NV_DMA_CH                  0
#define HAL_DMA_CH_RX                  3
#define HAL_DMA_CH_TX                  4

#define HAL_NV_DMA_GET_DESC()  HAL_DMA_GET_DESC0()
#define HAL_NV_DMA_SET_ADDR(a) HAL_DMA_SET_ADDR_DESC0((a))

/* Critical Vdd Monitoring to prevent flash damage or radio lockup. */

// Vdd/3 / Internal Reference X ENOB --> (Vdd / 3) / 1.15 X 127
#define VDD_2_0  74   // 2.0 V required to safely read/write internal flash.
#define VDD_2_7  100  // 2.7 V required for the Numonyx device.

#define VDD_MIN_RUN   VDD_2_0
#define VDD_MIN_NV   (VDD_2_0+4)  // 5% margin over minimum to survive a page erase and compaction.
#define VDD_MIN_XNV  (VDD_2_7+5)  // 5% margin over minimum to survive a page erase and compaction.

/*******************************************************************************
 * MACROS
 */

/* Cache Prefetch Control */

#define PREFETCH_ENABLE()  st( FCTL = 0x08; )
#define PREFETCH_DISABLE() st( FCTL = 0x04; )

/* Setting Clocks */

// switch to the 16MHz HSOSC and wait until it is stable
#define SET_OSC_TO_HSOSC()                                                     \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_16MHZ;                            \
  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_16MHZ );                            \
}

// switch to the 32MHz XOSC and wait until it is stable
#define SET_OSC_TO_XOSC()                                                      \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & 0x80) | CLKCONCMD_32MHZ;                            \
  while ( (CLKCONSTA & ~0x80) != CLKCONCMD_32MHZ );                            \
}

// set 32kHz OSC and wait until it is stable
#define SET_32KHZ_OSC()                                                        \
{                                                                              \
  CLKCONCMD = (CLKCONCMD & ~0x80) | OSC_32KHZ;                                 \
  while ( (CLKCONSTA & 0x80) != OSC_32KHZ );                                   \
}

#define START_HSOSC_XOSC()                                                     \
{                                                                              \
  SLEEPCMD &= ~OSC_PD;            /* start 16MHz RCOSC & 32MHz XOSC */         \
  while (!(SLEEPSTA & XOSC_STB)); /* wait for stable 32MHz XOSC */             \
}

#define STOP_HSOSC()                                                           \
{                                                                              \
  SLEEPCMD |= OSC_PD;             /* stop 16MHz RCOSC */                       \
}

/* Board Initialization */
#define HAL_BOARD_INIT()                                                       \
{                                                                              \
  /* Set to 16Mhz to set 32kHz OSC, then back to 32MHz */                      \
  START_HSOSC_XOSC();                                                          \
  SET_OSC_TO_HSOSC();                                                          \
  SET_32KHZ_OSC();                                                             \
  SET_OSC_TO_XOSC();                                                           \
  STOP_HSOSC();                                                                \
                                                                               \
  /* Enable cache prefetch mode. */                                            \
  PREFETCH_ENABLE();                                                           \
}

/* Debounce */
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }

/* ----------- Push Buttons ---------- */
#define HAL_PUSH_BUTTON1()        (PUSH1_POLARITY (PUSH1_SBIT))
#define HAL_PUSH_BUTTON2()        (PUSH2_POLARITY (PUSH2_SBIT))
#define HAL_PUSH_BUTTON3()        (0)
#define HAL_PUSH_BUTTON4()        (0)
#define HAL_PUSH_BUTTON5()        (0)
#define HAL_PUSH_BUTTON6()        (0)

/* LED's */

#if defined (HAL_BOARD_CC2530EB_REV17) && !defined (HAL_PA_LNA) && !defined (HAL_PA_LNA_CC2590)

  #define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
  #define HAL_TURN_OFF_LED2()       st( LED2_SBIT = LED2_POLARITY (0); )
  #define HAL_TURN_OFF_LED3()       st( LED3_SBIT = LED3_POLARITY (0); )
  #define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

  #define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
  #define HAL_TURN_ON_LED2()        st( LED2_SBIT = LED2_POLARITY (1); )
  #define HAL_TURN_ON_LED3()        st( LED3_SBIT = LED3_POLARITY (1); )
  #define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

  #define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
  #define HAL_TOGGLE_LED2()         st( if (LED2_SBIT) { LED2_SBIT = 0; } else { LED2_SBIT = 1;} )
  #define HAL_TOGGLE_LED3()         st( if (LED3_SBIT) { LED3_SBIT = 0; } else { LED3_SBIT = 1;} )
  #define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

  #define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
  #define HAL_STATE_LED2()          (LED2_POLARITY (LED2_SBIT))
  #define HAL_STATE_LED3()          (LED3_POLARITY (LED3_SBIT))
  #define HAL_STATE_LED4()          HAL_STATE_LED1()

#elif defined (HAL_BOARD_CC2530EB_REV13) || defined (HAL_PA_LNA) || defined (HAL_PA_LNA_CC2590)

  #define HAL_TURN_OFF_LED1()       st( LED1_SBIT = LED1_POLARITY (0); )
  #define HAL_TURN_OFF_LED2()       HAL_TURN_OFF_LED1()
  #define HAL_TURN_OFF_LED3()       HAL_TURN_OFF_LED1()
  #define HAL_TURN_OFF_LED4()       HAL_TURN_OFF_LED1()

  #define HAL_TURN_ON_LED1()        st( LED1_SBIT = LED1_POLARITY (1); )
  #define HAL_TURN_ON_LED2()        HAL_TURN_ON_LED1()
  #define HAL_TURN_ON_LED3()        HAL_TURN_ON_LED1()
  #define HAL_TURN_ON_LED4()        HAL_TURN_ON_LED1()

  #define HAL_TOGGLE_LED1()         st( if (LED1_SBIT) { LED1_SBIT = 0; } else { LED1_SBIT = 1;} )
  #define HAL_TOGGLE_LED2()         HAL_TOGGLE_LED1()
  #define HAL_TOGGLE_LED3()         HAL_TOGGLE_LED1()
  #define HAL_TOGGLE_LED4()         HAL_TOGGLE_LED1()

  #define HAL_STATE_LED1()          (LED1_POLARITY (LED1_SBIT))
  #define HAL_STATE_LED2()          HAL_STATE_LED1()
  #define HAL_STATE_LED3()          HAL_STATE_LED1()
  #define HAL_STATE_LED4()          HAL_STATE_LED1()

#endif

/* XNV */

#define XNV_SPI_BEGIN()             st(P1_3 = 0;)
#define XNV_SPI_TX(x)               st(U1CSR &= ~0x02; U1DBUF = (x);)
#define XNV_SPI_RX()                U1DBUF
#define XNV_SPI_WAIT_RXRDY()        st(while (!(U1CSR & 0x02));)
#define XNV_SPI_END()               st(P1_3 = 1;)

// The TI reference design uses UART1 Alt. 2 in SPI mode.
#define XNV_SPI_INIT() \
st( \
  /* Mode select UART1 SPI Mode as master. */\
  U1CSR = 0; \
  \
  /* Setup for 115200 baud. */\
  U1GCR = 11; \
  U1BAUD = 216; \
  \
  /* Set bit order to MSB */\
  U1GCR |= BV(5); \
  \
  /* Set UART1 I/O to alternate 2 location on P1 pins. */\
  PERCFG |= 0x02;  /* U1CFG */\
  \
  /* Select peripheral function on I/O pins but SS is left as GPIO for separate control. */\
  P1SEL |= 0xE0;  /* SELP1_[7:4] */\
  /* P1.1,2,3: reset, LCD CS, XNV CS. */\
  P1SEL &= ~0x0E; \
  P1 |= 0x0E; \
  P1_1 = 0; \
  P1DIR |= 0x0E; \
  \
  /* Give UART1 priority over Timer3. */\
  P2SEL &= ~0x20;  /* PRI2P1 */\
  \
  /* When SPI config is complete, enable it. */\
  U1CSR |= 0x40; \
  /* Release XNV reset. */\
  P1_1 = 1; \
)

/* Driver Configuration */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC TRUE
#endif

/* Set to TRUE enable DMA usage, FALSE disable it */
#ifndef HAL_DMA
#define HAL_DMA TRUE
#endif

/* Set to TRUE enable Flash access, FALSE disable it */
#ifndef HAL_FLASH
#define HAL_FLASH TRUE
#endif

/* Set to TRUE enable AES usage, FALSE disable it */
#ifndef HAL_AES
#define HAL_AES TRUE
#endif

#ifndef HAL_AES_DMA
#define HAL_AES_DMA TRUE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD TRUE
#endif

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED TRUE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY TRUE
#endif

/* Set to TRUE enable UART usage, FALSE disable it */
#ifndef HAL_UART
#if (defined ZAPP_P1) || (defined ZAPP_P2) || (defined ZTOOL_P1) || (defined ZTOOL_P2)
#define HAL_UART TRUE
#else
#define HAL_UART FALSE
#endif
#endif

#if HAL_UART
// Always prefer to use DMA over ISR.
#if HAL_DMA
  #ifndef HAL_UART_DMA
    #if (defined ZAPP_P1) || (defined ZTOOL_P1)
      #define HAL_UART_DMA  1
    #elif (defined ZAPP_P2) || (defined ZTOOL_P2)
      #define HAL_UART_DMA  2
    #else
      #define HAL_UART_DMA  1
    #endif
  #endif
  #ifndef HAL_UART_ISR
    #define HAL_UART_ISR  0
  #endif
#else
  #ifndef HAL_UART_ISR
    #if (defined ZAPP_P1) || (defined ZTOOL_P1)
      #define HAL_UART_ISR  1
    #elif (defined ZAPP_P2) || (defined ZTOOL_P2)
      #define HAL_UART_ISR  2
    #else
      #define HAL_UART_ISR  1
    #endif
  #endif
  #ifndef HAL_UART_DMA
    #define HAL_UART_DMA  0
  #endif
#endif

// Used to set P2 priority - USART0 over USART1 if both are defined.
#if ((HAL_UART_DMA == 1) || (HAL_UART_ISR == 1))
#define HAL_UART_PRIPO             0x00
#else
#define HAL_UART_PRIPO             0x40
#endif

#else
#define HAL_UART_DMA  0
#define HAL_UART_ISR  0
#endif

#if !defined HAL_UART_SPI
#define HAL_UART_SPI  0
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_BOARD_CFG_H */

/*******************************************************************************
*/
