/**
 * \file main.c      \brief MODBUS relay control main entry point
 */

#include "app.h"

#define TRACE_TAG "Main"
#if !ENABLE_TRACE_MAIN
#undef TRACE
#define TRACE(...)
#endif

// Prototypes:
static int modbus_rtu_recv_cb(hal_uart_t uart, uint8_t addr, uint8_t func, uint16_t regaddr, uint16_t regdata);

static uint16_t relays_state = 0;

int main(void)
{
   // Initialize HW board
   VERIFY_FATAL(board_init() == 0);

   // Initialize modbus RTU reciever
   VERIFY_FATAL(modbus_rtu_init(CFG_UART_MODBUS, CFG_UART_MODBUS_BAUDRATE, CFG_UART_MODBUS_SETINGS, CFG_UART_MODBUS_RX_TIMEOUT) == 0);
   VERIFY_FATAL(modbus_rtu_recv(CFG_UART_MODBUS, CFG_MODBUS_ADDR, modbus_rtu_recv_cb) == 0);

   while(1)
   {
      os_scheduler_schedule();
   }

   return 0;
}

static int modbus_rtu_recv_cb(hal_uart_t uart, uint8_t addr, uint8_t func, uint16_t regaddr, uint16_t regdata)
{
   switch(func)
   {
      case MODBUS_FUNC_READ_COILS:
      {
         int len = 0;
         uint8_t data[4];
         
         data[len++] = 2;                             // Number of bytes
         data[len++] = relays_state & 0xFF;           // Relay state LSB
         data[len++] = (relays_state >> 8) & 0xFF;    // Relay state MSB
         
         return modbus_rtu_write_response(uart, addr, func, data, len);
      }
      break;
      
      case MODBUS_FUNC_WRITE_COIL:
      {
         int len = 0;
         uint8_t data[4];
         
         if (regaddr > 15)
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
             
         return modbus_rtu_write_response(uart, addr, func, data, len);
      }
      break;
         
      default:
         return -1;
   }
   
   return 0;
}
