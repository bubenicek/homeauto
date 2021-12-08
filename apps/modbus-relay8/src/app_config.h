
#ifndef __CONFIG_H
#define __CONFIG_H

#define CFG_FW_VERSION_MAJOR        0
#define CFG_FW_VERSION_MINOR        1

#define CFG_HW_VERSION_MODEL        0
#define CFG_HW_VERSION_REVISION     1

#define CFG_OPENOS_OS_API           1


//-----------------------------------------------------------------------------
//                      MODBUS configuration
//-----------------------------------------------------------------------------
#define CFG_UART_MODBUS                  HAL_UART0
#define CFG_UART_MODBUS_BAUDRATE         9600
#define CFG_UART_MODBUS_SETINGS          0
#define CFG_MODBUS_USE_RX_CALBACK        1

#define CFG_MODBUS_ADDR                  0x3

/** Idle receive timeout */
#define CFG_MODBUS_RX_TIMEOUT            3



#endif