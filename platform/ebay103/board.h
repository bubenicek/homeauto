
#ifndef __NATIVE_BOARD_H
#define __NATIVE_BOARD_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>


#include "stm32f1xx_hal.h"


//
// Board configuration
//

#define ENABLE_INTERRUPTS()                __asm__("CPSIE I\n")
#define DISABLE_INTERRUPTS()               __asm__("CPSID I\n")

// Determine whether we are in thread mode or handler mode.
#define IRQ_MODE() (__get_IPSR() != 0)

//
// GPIO configuration
//
#define CFG_HAL_GPIO_DEF { \
   {GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1},         /* LED system */ \
   {GPIOB, GPIO_PIN_6 , GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1},         /* LED error */   \
   {GPIOC, GPIO_PIN_14, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1},         /* LED data */   \
   {GPIOB, GPIO_PIN_4,  GPIO_MODE_IT_RISING_FALLING, GPIO_NOPULL, 0}, /* BTN MODE */   \
   {GPIOA, GPIO_PIN_0,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0},         /* DF RESET */   \
   {GPIOB, GPIO_PIN_5,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1},         /* USB PULLUP */ \
   {GPIOA, GPIO_PIN_4,  GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1},         /* SPI CS */     \
}


//
// LED configuration
//
#define CFG_HAL_LED_DEF  { \
   {HAL_GPIO0, 0},          /* LED system */  \
   {HAL_GPIO1, 0},          /* LED error */   \
   {HAL_GPIO2, 0},          /* LED data */    \
}



//
// Console configuration
//
#define DEBUG_TX_Pin          GPIO_PIN_9
#define DEBUG_TX_GPIO_Port    GPIOA
#define DEBUG_RX_Pin          GPIO_PIN_10
#define DEBUG_RX_GPIO_Port    GPIOA


//
// UART0 configuration
//
#define SDATA_TX_Pin          GPIO_PIN_2
#define SDATA_TX_GPIO_Port    GPIOA
#define SDATA_RX_Pin          GPIO_PIN_3
#define SDATA_RX_GPIO_Port    GPIOA


//
// IRQ preemtion priority (lower is higher priority)
//
#define UART_SDATA_PRIORITY   0
#define BTN_MODE_PRIORITY     6
#define SYSTICK_PRIORITY      15

//
// Board peripherials configuration
//

#define LED_DEBUG             HAL_LED0
#define LED_ERROR             HAL_LED1
#define LED_DATA              HAL_LED2
#define LED_SYSTEM            HAL_LED0

#define BTN_MODE              HAL_GPIO3

// SPI dataflash
#define DF_SPI                HAL_SPI0
#define DF_SPI_CS             HAL_GPIO6
#define DF_SPI_GPIO_RESET     HAL_GPIO4

#define USB_PULLUP            HAL_GPIO5

#define UART_SDATA            HAL_UART0
#define UART_SDATA_BAUDRATE   9600

// Enable USB mass storage drivers
#define CFG_USB_STORAGE       1

#endif   // __NATIVE_BOARD_H
