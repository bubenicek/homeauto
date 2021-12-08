/**
 * \file main.c      \brief test serial console
 */

#include "system.h"
#include "ebus.h"
#include "bcd.h"

#define TRACE_TAG "Main"
#if !ENABLE_TRACE_MAIN
#undef TRACE
#define TRACE(...)
#endif

#define CFG_EBUS_UART         HAL_UART0
#define CFG_EBUS_SRC_ADDR     0xFF

// Locals:
static ebus_t ebus;


static void test_send_raw(void)
{
   int nerr = 0;

   ebus_response_packet_t resp;
/*
   ebus_request_packet_t req = {
      .src = CFG_EBUS_SRC_ADDR,
      .dst = 0x15,
      .cmd_primary = 0xB5,
      .cmd_second = 0x09,
      .datalen = 0x03,
      .data = {0x0D, 0x62, 0x00}
   };
*/
   ebus_request_packet_t req = {
      .src = CFG_EBUS_SRC_ADDR,
      .dst = 0x15,
      .cmd_primary = 0xB5,
      .cmd_second = 0x09,
      .datalen = 0x04,
      .data = {0x0E,0x42,0x00,0x00}
   };


   while(1)
   {
      if (ebus_sendrecv_packet(&ebus, &req, &resp) == 0)
      {
         TRACE("RX %d bytes nerr: %d", resp.datalen, nerr);

         uint16_t bcd_value = (resp.data[1] << 8) | resp.data[0];
         int raw_value = bcd2dec(bcd_value);
         float float_value = raw_value / 10.0;

         TRACE("VALUE:  %d (%d) %2.2f", bcd_value, raw_value, float_value);
         break;

      }
      else
      {
         nerr++;
      }

      hal_delay_ms(rand() % 1000);
   }
}

static int test_send_commands(void)
{
    while(1)
    {
        ebus_value_t param, value;

        ebus_value_set_null(&param);

        if (ebus_sendrecv_command(&ebus, "vrc430.temp_d_actual_dhw", &param, &value) == 0)
        {
            TRACE("temp_d_actual_dhw: %s", ebus_value_str(&value));
        }

/*
        if (ebus_sendrecv_command(&ebus, "vrc430.temp_d_dhw", &param, &value) == 0)
        {
            TRACE("temp_d_dhw: %s", ebus_value_str(&value));
        }

        if (ebus_sendrecv_command(&ebus, "vrc430.temp_room_disp", &param, &value) == 0)
        {
            TRACE("temp_room_disp: %s", ebus_value2str(&value));
        }

        if (ebus_sendrecv_command(&ebus, "vrc430.temp_outside", &param, &value) == 0)
        {
            TRACE("temp_outside: %s", ebus_value2str(&value));
        }
*/


/*
        ebus_value_set_null(&param);
        if (ebus_sendrecv_command(&ebus, "vrc430.program_dhw_circuit", &param, &value) == 0)
        {
            TRACE("program_dhw_circuit: %s", ebus_value_str(&value));
        }

        ebus_value_set_byte(&param, 2);
        if (ebus_sendrecv_command(&ebus, "vrc430.set_program_dhw_circuit", &param, &value) == 0)
        {
            TRACE("set_program_dhw_circuit ok, resp: %s",  ebus_value_str(&value));
        }
*/
        hal_delay_ms(1000);
    }

    return 0;
}

int main(void)
{
   // Initialize HW board
   VERIFY_FATAL(board_init() == 0);

   // Initialize EBUS link
   VERIFY_FATAL(ebus_init(&ebus, CFG_EBUS_UART, CFG_EBUS_SRC_ADDR) == 0);

   TRACE("EBUS test is running ...");

   //test_send_raw();
   test_send_commands();

   return 0;
}
