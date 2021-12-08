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

// Locals:
static os_timer_t timerstat;
static console_t console;


static void statinfo_task(void *arg)
{
    hal_led_blink(LED_SYSTEM, 2, 100, 250);
}

int main(void)
{
    // Initialize HW board
    VERIFY_FATAL(board_init() == 0);

    // Initialize FAN PWM
    VERIFY_FATAL(fanpwm_init() == 0);

    // Initialize USB serial console
    VERIFY_FATAL(console_init(&console) == 0);

    // Start periodical LED blink
    os_timer_start(&timerstat, OS_TIMER_PERIODICAL, 1000, statinfo_task, NULL);

    TRACE("FAN controller ver. %d.%d is running", APP_VERSION_MAJOR(app_header.fw_version), APP_VERSION_MINOR(app_header.fw_version));

    while (1) 
    {
        os_scheduler_schedule();
    }

    return 0;
}
