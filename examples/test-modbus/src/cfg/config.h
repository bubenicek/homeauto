
#ifndef __CONFIG_H
#define __CONFIG_H

#define CFG_FW_VERSION_MAJOR        0
#define CFG_FW_VERSION_MINOR        1

#define CFG_HW_VERSION_MODEL        0
#define CFG_HW_VERSION_REVISION     1

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

#define ENABLE_TRACE_MAIN           1
#define ENABLE_TRACE_MODBUS         1

#define CFG_MODBUS_ADDR             0x2


#endif   // __CONFIG_H
