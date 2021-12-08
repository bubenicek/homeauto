
#ifndef __NATIVE_BOARD_H
#define __NATIVE_BOARD_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/time.h>

#include "stm32f1xx_hal.h"


//
// Board configuration
//

#define CFG_HW_VERSION_MODEL              0
#define CFG_HW_VERSION_REVISION           1

#define ENABLE_INTERRUPTS()                __asm__("CPSIE I\n")
#define DISABLE_INTERRUPTS()               __asm__("CPSID I\n")

// Determine whether we are in thread mode or handler mode. 
#define IRQ_MODE() (__get_IPSR() != 0)

#define CFG_HAL_SYSTICK_PRIORITY    0
#define CFG_HAL_GPIO_PRIORITY       0

//
// GPIO configuration
//
#define CFG_HAL_GPIO_DEF { \
   {GPIOC, GPIO_PIN_13, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 1},         /* LED system */       \
   {GPIOA, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0},          /* RS485 direction */  \
}   

//
// LED configuration
//
#define CFG_HAL_LED_DEF  {  \
   {HAL_GPIO0, 0},          /* LED system */ \
}


   
//
// Console configuration
//
#define DEBUG_UART            USART1
#define DEBUG_TX_Pin          GPIO_PIN_9
#define DEBUG_TX_GPIO_Port    GPIOA
#define DEBUG_RX_Pin          GPIO_PIN_10
#define DEBUG_RX_GPIO_Port    GPIOA
 

//
// Board peripherials configuration
//

#define LED_SYSTEM             HAL_LED0
#define LED_ERROR              HAL_LED0

#endif   // __BOARD_H
