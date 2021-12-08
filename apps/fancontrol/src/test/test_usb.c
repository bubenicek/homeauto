
#include "system.h"

TRACE_TAG(test_usb);


static void test_usbserial_task(void *arg)
{
   int c;

   if ((c = hal_usbserial_getchar(HAL_USB0)) != -1)
   {
      hal_usbserial_putchar(HAL_USB0, c);
   }

   os_scheduler_push_task(test_usbserial_task, 0, NULL);
}

int test_usb(void)
{
    // Initialize USB serial port 
    VERIFY_FATAL(hal_usbserial_init(HAL_USB0) == 0);
    os_scheduler_push_task(test_usbserial_task, 0, NULL);

    return 0;
}
