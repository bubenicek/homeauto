/**
 * \file main.c      \brief LED strip controller entry point
 */

#include "system.h"

#include "network.h"
#include "ledctl.h"

#define TRACE_TAG "Main"
#if !ENABLE_TRACE_MAIN
#undef TRACE
#define TRACE(...)
#endif

int main(void)
{
    // Initialize HW board
    VERIFY_FATAL(board_init() == 0);
    VERIFY_FATAL(network_init() == 0);
    VERIFY_FATAL(ledctl_init() == 0);

    TRACE("LED strip controller is running ...");

    while(1)
    {
        hal_gpio_toggle(HAL_GPIO0);
        hal_delay_ms(network_is_connected() ? 1000 : 250);
    }

   return 0;
}

