
#include "system.h"


static volatile uint8_t inputs_state = 0;

void Delay(uint32_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

static void hal_uart_recv_cb(hal_uart_t uart, uint8_t c)
{
   hal_uart_putchar(uart, c);
}

static void gpio_input_changed_cb(hal_gpio_t gpio)
{
   int index;
   
   inputs_state = 0;
   
   // Read states from all inputs
   for(index = 0, gpio = GPIO_INPUT1; gpio <= GPIO_INPUT8; gpio++, index++)
   {
      if (hal_gpio_get(gpio))
         inputs_state |= (1 << index);
   }
}

int main(void)
{
   board_init();

   hal_uart_init(UART_MODBUS, UART_MODBUS_BAUDRATE, 0);
   hal_uart_recv(UART_MODBUS, hal_uart_recv_cb);

   hal_gpio_register_irq_handler(GPIO_INPUT1, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT2, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT3, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT4, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT5, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT6, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT7, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT8, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);

   while (1)
   {
      hal_gpio_toggle(HAL_GPIO16);  
      hal_uart_putchar(HAL_UART0, inputs_state);
      hal_delay_ms(500);
   }    
}
