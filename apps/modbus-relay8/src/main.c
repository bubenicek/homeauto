
#include "system.h"
#include "modbus.h"

// Prototypes:
static void gpio_input_changed_cb(hal_gpio_t gpio);
static int modbus_rtu_recv_cb(hal_uart_t uart, uint8_t addr, uint8_t func, uint16_t regaddr, uint16_t regdata);

// Locals:
static volatile uint8_t inputs_state = 0;
static uint8_t relays_state = 0;



int main(void)
{
   VERIFY_FATAL(board_init() == 0);

   // Initialize modbus RTU reciever
   VERIFY_FATAL(modbus_rtu_init(CFG_UART_MODBUS, CFG_UART_MODBUS_BAUDRATE, CFG_UART_MODBUS_SETINGS, CFG_MODBUS_RX_TIMEOUT) == 0);
   VERIFY_FATAL(modbus_rtu_recv(CFG_UART_MODBUS, CFG_MODBUS_ADDR, modbus_rtu_recv_cb) == 0);

   // Register inputs IRQ callbacks
   hal_gpio_register_irq_handler(GPIO_INPUT1, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT2, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT3, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT4, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT5, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT6, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT7, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);
   hal_gpio_register_irq_handler(GPIO_INPUT8, HAL_GPIO_IRQ_EDGE_RISINGFALLING, gpio_input_changed_cb);

   hal_led_blink(LED_SYSTEM, 3, 50, 250);

   while(1)
   {
      os_scheduler_schedule();
   }
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

static int modbus_rtu_recv_cb(hal_uart_t uart, uint8_t addr, uint8_t func, uint16_t regaddr, uint16_t regdata)
{
   int res;
  
   switch(func)
   {
      case MODBUS_FUNC_READ_COILS:
      {
         int len = 0;
         uint8_t data[2];
         
         data[len++] = 1;                             // Number of bytes
         data[len++] = relays_state;                  // Relay state
         
         res = modbus_rtu_write_response(uart, addr, func, data, len);
      }
      break;
      
      case MODBUS_FUNC_WRITE_COIL:
      {
         int len = 0;
         uint8_t data[4];
         
         if (regaddr > 8)
            return -1;
         
         data[len++] = (regaddr >> 8) & 0xFF;
         data[len++] = regaddr & 0xFF;
         data[len++] = (regdata >> 8) & 0xFF;
         data[len++] = regdata & 0xFF;
         
         if (regdata == 0)
         {
            relays_state &= ~(1 << regaddr);
            hal_gpio_set(regaddr, 1);
         }
         else
         {
            relays_state |= (1 << regaddr);
            hal_gpio_set(regaddr, 0);
         }
             
         res = modbus_rtu_write_response(uart, addr, func, data, len);
      }
      break;
      
      case MODBUS_READ_DISCRETE_INPUTS:
      {
         int len = 0;
         uint8_t data[2];
         
         data[len++] = 1;                             // Number of bytes
         data[len++] = inputs_state;                  // Inputs state
         
         res = modbus_rtu_write_response(uart, addr, func, data, len);
      }
      break;
         
      default:
         res = -1;
   }
   
   return res;
}