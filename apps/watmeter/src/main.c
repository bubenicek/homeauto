/**
 * \file main.c      \brief Application entry point
 */

#include "app.h"

TRACE_TAG(Main)
#if !ENABLE_TRACE_MAIN
#undef TRACE
#define TRACE(...)
#endif

int __errno;

// Application header
__attribute__((section(".app_header"))) const app_header_t app_header =
{
   .magic = APP_HEADER_MAGIC,
   .hw_version = APP_VERSION(CFG_HW_VERSION_MODEL, CFG_HW_VERSION_REVISION),
   .fw_version = APP_VERSION(CFG_FW_VERSION_MAJOR, CFG_FW_VERSION_MINOR),
   .fw_size = 0,
   .fw_crc = 0,
};

// Global objects
emon_task_t emon;
os_timer_t timerstat;


static void statinfo_task(void *arg)
{
   double irms = emon_task_get_irms(&emon);

   TRACE("IRMS: %g   W: %g  %d", irms, irms * 220.0, emon.irms_count);
   hal_led_blink(LED_SYSTEM, 1, 50, 100);
}


static void hal_uart_rs485_enable_tx(hal_uart_t uart, uint8_t enable)
{
   if (enable)
   {
      // Enable TX
      hal_gpio_set(GPIO_RS485_DIR, 1);
   }
   else
   {
      hal_uart_sync(uart);

      // Enable RX
      hal_gpio_set(GPIO_RS485_DIR, 0);
   }
}


int main(void)
{
   int c;

   // Initialize HW board
   VERIFY_FATAL(board_init() == 0);
   
   VERIFY_FATAL(hal_uart_init(HAL_UART0) == 0);

   // Start emonitor
   VERIFY_FATAL(emon_task_start(&emon) == 0);

   os_timer_start(&timerstat, OS_TIMER_PERIODICAL, 100, statinfo_task, NULL);
   
   TRACE("Watmeter ver. %d.%d is running", APP_VERSION_MAJOR(app_header.fw_version), APP_VERSION_MINOR(app_header.fw_version));
    
    
   while(1)
   {
      // Enable RX
      hal_uart_rs485_enable_tx(HAL_UART0, 0);

      if ((c = hal_uart_getchar(HAL_UART0)) != -1)
      {
         // Enable TX
         hal_uart_rs485_enable_tx(HAL_UART0, 1);

         hal_uart_putchar(HAL_UART0, c);
         hal_led_toggle(HAL_LED0);
      }
   }
      
   while(1)
   {
      os_scheduler_schedule();
   }

   return 0;
}
