
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
#define CFG_DEBUG_HAS_FLOAT         1
#define CFG_DEBUG_TIMESTAMP         1
#define CFG_CMSIS_OS_API            0
#define CFG_OPENOS_OS_API           0
#define CFG_HAL_WDG_ENABLED         1

//-----------------------------------------------------------------------------
//                      Debug trace configuration
//-----------------------------------------------------------------------------
#define CFG_ENABLE_TRACE            1
#define ENABLE_TRACE_MAIN           1
#define ENABLE_TRACE_MODBUS         1
#define ENABLE_TRACE_MODBUS_DATA    1


//-----------------------------------------------------------------------------
//                      MODBUS configuration
//-----------------------------------------------------------------------------
#define CFG_UART_MODBUS                  HAL_UART0
#define CFG_UART_MODBUS_BAUDRATE         19200
#define CFG_UART_MODBUS_SETINGS          0
#define CFG_UART_MODBUS_RX_TIMEOUT       1000
#define CFG_MODBUS_USE_RX_CALBACK        0
#define CFG_MODBUS_DELAY_AFTER_TX    	 10


#endif   // __CONFIG_H
