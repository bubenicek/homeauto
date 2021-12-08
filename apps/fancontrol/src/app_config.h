
#ifndef __CONFIG_H
#define __CONFIG_H

#define CFG_FW_VERSION_MAJOR        0
#define CFG_FW_VERSION_MINOR        6


//-----------------------------------------------------------------------------
//                      System configuration
//-----------------------------------------------------------------------------
#define CFG_DEBUG_MEM               1
#define CFG_DEBUG_TIMESTAMP         1
#define CFG_CMSIS_OS_API            0
#define CFG_OPENOS_OS_API           1
#define CFG_HAL_WDG_ENABLED         1

//-----------------------------------------------------------------------------
//                      Debug trace configuration
//-----------------------------------------------------------------------------
#define CFG_ENABLE_TRACE            1

#define ENABLE_TRACE_HAL            1
#define ENABLE_TRACE_MAIN           1


//
// Console driver
//

#define CFG_CONSOLE_USE_PROMPT      0       // Dont use prompt

#define console_driver_init(s)      hal_usbserial_init(HAL_USB0)
#define console_putchar(s,c)        hal_usbserial_putchar(HAL_USB0, c)
#define console_getchar(s)          hal_usbserial_getchar(HAL_USB0)


#endif   // __CONFIG_H
