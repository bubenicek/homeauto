/*******************************************************************************
*  Filename:        hal_uart_usb.c
*  Revised:         $Date: 2014-01-10 11:47:31 +0100 (fr, 10 jan 2014) $
*  Revision:        $Revision: 11711 $
*
*  Description:     UART interface for USB virtual COMPORT. Supports hardware 
*                   flow control.
*
*
*  Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************/


/******************************************************************************
* INCLUDES
*/
#include "hal_types.h"
#include "hal_uart.h"
#include "hal_board.h"
#include "util_buffer.h"
#include "hal_int.h"
#include "hal_mcu.h"
#include "usb_uart.h"

/******************************************************************************
* LOCAL CONSTANTS and DEFINITIONS
*/



/******************************************************************************
* GLOBAL VARIABLES
*/
ringBuf_t rbRxBuf;
ringBuf_t rbTxBuf;

extern uint8 cdcCTS;

/******************************************************************************
* LOCAL VARIABLES
*/

/**************************************************************************//**
* @brief   Initalise UART. Supported baudrates are: 38400, 57600 and 115200
*
* @param   uint8 baudrate
*          uint8 options - this parameter is ignored
*
* @return  none
*/
void halUartInit(uint8 baudrate, uint8 options)
{
    uint32 baud;

    // Initialize the buffers
    bufInit(&rbRxBuf);
    bufInit(&rbTxBuf);

    switch (baudrate) 
    {
    case HAL_UART_BAUDRATE_38400:
        baud= 38400;
        break;
    case HAL_UART_BAUDRATE_57600:
        baud= 57600;
        break;
    default:
        baud= 115200;
        break;
    }
    usbUartInit(baud);
}


/**************************************************************************//**
* @brief   Write data buffered to UART. Data is written into a buffer, and the
*          buffer is emptied by UART TX interrupt ISR
*
* @param   uint8* buf - buffer with data to write
*          uint8 length - number of bytes to write
*
* @return  number of bytes written
*/
uint8 halUartBufferedWrite(const uint8* buf, uint8 length)
{
    return (bufPut(&rbTxBuf,buf,length));
}


/**************************************************************************//**
* @brief   Write data buffer to UART
*
* @param   uint8* buf - buffer with data to write
*          uint8 length - number of bytes to write
*
* @return  number of bytes written
*/
uint8 halUartWrite(const uint8* buf, uint8 length)
{
    return halUartBufferedWrite(buf, length);
}


/**************************************************************************//**
* @brief   Read data from UART Rx buffer
*
* @param   uint8* buf - buffer with data to read in to
*          uint16 length - number of bytes to read
*
* @return  none
*/
uint8 halUartRead(uint8* buf, uint8 length)
{
    return bufGet(&rbRxBuf, buf, length);
}


/**************************************************************************//**
* @brief   Returns number of bytes in RX buffer
*
* @param   none
*
* @return  number of bytes in RX buffer
*/
uint8 halUartGetNumRxBytes(void)
{
    return bufNumBytes(&rbRxBuf);
}


/**************************************************************************//**
* @brief   Signal ready/not ready to receive characters on UART
*
* @param   uint8 enable - TRUE to signal ready to receive on UART
*                         FALSE to signal not ready to receive on UART
*
* @return  none
*/
void halUartEnableRxFlow(uint8 enable)
{
    cdcCTS= enable;
}
