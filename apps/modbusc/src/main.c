/**
 * \file main.c      \brief MODBUS relay control main entry point
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "system.h"
#include "modbus.h"

#define TRACE_TAG "Main"

typedef enum
{
   ACTION_NONE,
   ACTION_READ_DISCRETE_INPUTS,         // 2
   ACTION_READ_COILS,                   // 1
   ACTION_WRITE_COILS,                  // 5
   ACTION_READ_HOLDING_REGISTER,        // 3
   ACTION_WRITE_SINGLE_REGISTER,        // 6

} action_t;


static void usage(void)
{
   printf("Usage modbusc [-options]\n");
   printf("Options:\n");
   printf("   -d <serial device number>         Serial device number (0 = /dev/ttyUSB0 ...)\n");
   printf("   -b <baudrate>                     Baudrate (default 19200)\n");
   printf("   -a <addr>                         Device address\n");
   printf("   -ri <start_input> <num_inputs>    Read discreate input - 0x2\n");
   printf("   -rc <start_coil> <num_coils>      Read coil - 0x1\n");
   printf("   -wc <start_coil> <state>          Write coil - 0x5\n");
   printf("   -wr <regaddr> <regdata>           Write single register - 0x6\n");
   printf("   -rr <regaddr> <numregs>           Read holding single register - 0x3\n");
}

int main(int argc, char *argv[])
{
   int ix;
   int uart = -1;
   int addr = -1;
   uint32_t baudrate = CFG_UART_MODBUS_BAUDRATE;
   uint16_t regaddr = 0;
   uint16_t regval = 0;
   uint16_t numregs = 0;
   action_t action = ACTION_NONE;

   // Initialize HW board
   VERIFY_FATAL(board_init() == 0);

   if (argc < 2)
   {
      usage();
      return 1;
   }

   for (ix = 1; ix < argc; ix++)
   {
      if (!strcmp(argv[ix], "-d"))
      {
         uart = atoi(argv[++ix]);
      }
      else if (!strcmp(argv[ix], "-b"))
      {
         baudrate = atoi(argv[++ix]);
      }
      else if (!strcmp(argv[ix], "-a"))
      {
         addr = atoi(argv[++ix]);
      }
      else if (!strcmp(argv[ix], "-ri"))
      {
         regaddr = atoi(argv[++ix]);
         numregs = atoi(argv[++ix]);
         action = ACTION_READ_DISCRETE_INPUTS;
      }
      else if (!strcmp(argv[ix], "-rc"))
      {
         regaddr = atoi(argv[++ix]);
         numregs = atoi(argv[++ix]);
         action = ACTION_READ_COILS;
      }
      else if (!strcmp(argv[ix], "-wr"))
      {
         regaddr = atoi(argv[++ix]);
         regval = atoi(argv[++ix]);
         action = ACTION_WRITE_SINGLE_REGISTER;
      }
      else if (!strcmp(argv[ix], "-rr"))
      {
         regaddr = atoi(argv[++ix]);
         numregs = atoi(argv[++ix]);
         action = ACTION_READ_HOLDING_REGISTER;
      }
      else
      {
         TRACE_ERROR("Not supported option '%s'", argv[ix]);
         return 1;
      }
   }

   if (uart == -1)
   {
      TRACE_ERROR("Not specified uart device number");
      return 1;
   }

   if (addr == -1)
   {
      TRACE_ERROR("Not specified device address");
      return 1;
   }

   // Initialize modbus
   VERIFY_FATAL(modbus_rtu_init(uart, baudrate, CFG_UART_MODBUS_SETINGS, CFG_UART_MODBUS_RX_TIMEOUT) == 0);

   switch(action)
   {
      case ACTION_READ_DISCRETE_INPUTS:
      {
         int ix;
         uint16_t regs[255];

         if (modbus_rtu_read_inputs(uart, addr, regaddr, numregs, regs) < 0)
         {
            TRACE_ERROR("modbus_rtu_read_inputs 0x%X = 0x%X to device addr: 0x%X failed", regaddr, regval, addr);
            return 1;
         }

         TRACE_PRINTF("[Read discreate inputs] ");
         for (ix = 0; ix < numregs; ix++)
         {
            TRACE_PRINTF("%d=0x%X ", regaddr+ix, regs[ix]);
         }
         TRACE_PRINTF("\n");
      }
      break;

      case ACTION_READ_COILS:
      {
         int ix;
         uint16_t regs[255];

         if (modbus_rtu_read_coils_state(uart, addr, regaddr, numregs, regs) < 0)
         {
            TRACE_ERROR("modbus_rtu_read_coils 0x%X = 0x%X to device addr: 0x%X failed", regaddr, regval, addr);
            return 1;
         }

         TRACE_PRINTF("[Read coils] ");
         for (ix = 0; ix < numregs; ix++)
         {
            TRACE_PRINTF("%d=0x%X ", regaddr+ix, regs[ix]);
         }
         TRACE_PRINTF("\n");
      }
      break;

      case ACTION_WRITE_SINGLE_REGISTER:
      {
         if (modbus_rtu_write_sigle_register(uart, addr, regaddr, regval) < 0)
         {
            TRACE_ERROR("Write single register 0x%X = 0x%X to device addr: 0x%X failed", regaddr, regval, addr);
            return 1;
         }
      }
      break;

      case ACTION_READ_HOLDING_REGISTER:
      {
         int ix;
         uint16_t regs[255];

         if (modbus_rtu_read_holding_register(uart, addr, regaddr, numregs, regs) < 0)
         {
            TRACE_ERROR("modbus_rtu_read_holding_register register 0x%X = 0x%X to device addr: 0x%X failed", regaddr, regval, addr);
            return 1;
         }

         TRACE_PRINTF("[Read holding register] ");
         for (ix = 0; ix < numregs; ix++)
         {
            TRACE_PRINTF("%d=0x%X ", regaddr+ix, regs[ix]);
         }
         TRACE_PRINTF("\n");
      }
      break;

      default:
         TRACE_ERROR("Unknown action");
         return 1;
   }

   return 0;
}
