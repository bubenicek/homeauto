/*******************************************************************************
*  Filename:        hal_uart_cc8051.c
*  Revised:         $Date: 2014-01-10 11:47:31 +0100 (fr, 10 jan 2014) $
*  Revision:        $Revision: 11711 $
*
*  Description:     UART interface to UART0 at P0. Works with HW flow control 
*                   and optionally with buffered TX transmission. Reception is 
*                   always buffered. Works with 8051 based SoCs.
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

/******************************************************************************
* LOCAL CONSTANTS and DEFINITIONS
*/

#define HAL_UART_TX_POLLING

#define P2DIR_PRIPO               0xC0
#define HAL_UART_PRIPO            0x00

#define HAL_UART_0_PERCFG_BIT     0x01  // USART0 on P0, so clear this bit.
#define HAL_UART_0_P0_RX_TX       0x0C  // Peripheral I/O Select for Rx/Tx.

// UxCSR - USART Control and Status Register.
#define CSR_MODE                  0x80
#define CSR_RE                    0x40
#define CSR_SLAVE                 0x20
#define CSR_FE                    0x10
#define CSR_ERR                   0x08
#define CSR_RX_BYTE               0x04
#define CSR_TX_BYTE               0x02
#define CSR_ACTIVE                0x01

// UxUCR - USART UART Control Register.
#define UCR_FLUSH                 0x80
#define UCR_FLOW                  0x40
#define UCR_D9                    0x20
#define UCR_BIT9                  0x10
#define UCR_PARITY                0x08
#define UCR_SPB                   0x04
#define UCR_STOP                  0x02
#define UCR_START                 0x01

#define UTX0IE                    0x04
#define UTX1IE                    0x08


/******************************************************************************
* LOCAL VARIABLES
*/
ringBuf_t rbRxBuf;
#ifndef HAL_UART_TX_POLLING
static ringBuf_t rbTxBuf;
#endif


// Baud rate settings
#if BSP_CONFIG_CLOCK_MHZ==32

#define BAUD_M_38400                 59
#define BAUD_M_57600                216
#define BAUD_M_115200               216

#define BAUD_E_38400                 10
#define BAUD_E_57600                 10
#define BAUD_E_115200                11

#elif BSP_CONFIG_CLOCK_MHZ==26

#define BAUD_M_38400                131
#define BAUD_M_57600                 34
#define BAUD_M_115200                34

#define BAUD_E_38400                 10
#define BAUD_E_57600                 11
#define BAUD_E_115200                12

#elif BSP_CONFIG_CLOCK_MHZ==24

#define BAUD_M_38400                163
#define BAUD_M_57600                 59
#define BAUD_M_115200                59

#define BAUD_E_38400                 10
#define BAUD_E_57600                 11
#define BAUD_E_115200                12

#else
#error "Clock speed not defined!"
#endif

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
    // Set P2 priority - USART0 over USART1 if both are defined.
    P2DIR &= ~P2DIR_PRIPO;
    P2DIR |= HAL_UART_PRIPO;
    
    // Set UART0 I/O location to P0.
    PERCFG &= ~HAL_UART_0_PERCFG_BIT;
    
    // Enable Tx and Rx on P0
    P0SEL |= (HAL_UART_0_P0_RX_TX);
    P1SEL &= ~(HAL_UART_0_P0_RX_TX);
    
    // Make sure ADC doesnt use this
    ADCCFG &= ~(HAL_UART_0_P0_RX_TX);
    
    U0CSR=  CSR_MODE;             // UART mode
    U0UCR|= UCR_FLUSH;
    
    // Set baud rate
    switch (baudrate) {
    case HAL_UART_BAUDRATE_38400:
        U0BAUD = BAUD_M_38400;
        U0GCR = BAUD_E_38400;
        break;
    case HAL_UART_BAUDRATE_57600:
        U0BAUD = BAUD_M_57600;
        U0GCR = BAUD_E_57600;
        break;
    default:
        // 115200 default
        U0BAUD = BAUD_M_115200;
        U0GCR = BAUD_E_115200;
    }
    
    
    // Initialize the RX buffer
    bufInit(&rbRxBuf);
    
#ifndef HAL_UART_TX_POLLING
    // Enable transmit register empty interrupt
    UTX0IF= 1;
    
    bufInit(&rbTxBuf);
#endif
    
    // Set RTS pin to output
    HAL_RTS_DIR_OUT();
    
    // One stop bit
    U0UCR |= UCR_STOP; 
    
    // Prepare for reception
    URX0IF = 0;       
    U0CSR |= CSR_RE;
    URX0IE = 1;
    
    // Enable HW flow control
    halUartEnableRxFlow(TRUE);
}


#ifndef HAL_UART_TX_POLLING
/**************************************************************************//**
* @brief   Write data buffered to UART. Data is written into a buffer, and the
*          buffer is emptied by UART TX interrupt ISR
*
* @param   uint8* buf - buffer with data to write
*          uint8 length - number of bytes to write
*
* @return  number of bytes written
*/
static uint8 halUartBufferedWrite(const uint8* buf, uint8 length)
{
    uint8 n;

    n= 0;
    while ( n < length ) {
        n+= bufPut(&rbTxBuf,buf+n,length-n);
    }

    // Enable UART TX interrupt
    IEN2|= UTX0IE;

    return n;
}


/**************************************************************************//**
* @brief   Write data buffer to UART
*
* @param   uint8* buf - buffer with data to write
*          uint8 length - number of bytes to write
*
* @return  uint8 - number of bytes written
*/
uint8 halUartWrite(const uint8* buf, uint8 length)
{
    return halUartBufferedWrite(buf,length);
}

#else

/**************************************************************************//**
* @brief   Write data buffer to UART
*
* @param   uint8* buf - buffer with data to write
*          uint8 length - number of bytes to write
*
* @return  uint8 - number of bytes written
*/
uint8 halUartWrite(const uint8* buf, uint8 length)
{
    uint16 i;

    UTX0IF = 0;
    for(i = 0; i < length; i++)
    {
        U0DBUF = buf[i];
        while (!UTX0IF);
        UTX0IF = 0;
    }
    return length;
}
#endif

/**************************************************************************//**
* @brief   Read data from UART Rx buffer
*
* @param   uint8* buf - buffer with data to read in to
*          uint8 length - number of bytes to read
*
* @return  number of bytes read
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
* @return  uint8
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
    // Enable RX flow
    if(enable) {
        HAL_RTS_CLR();
    }
    // Disable RX flow
    else {
        HAL_RTS_SET();
    }
}

/**************************************************************************//**
* @brief   ISR framework for the USCI A0/B0 Receive component
*
* @param   none
*
* @return  none
*/
HAL_ISR_FUNCTION(uart0RxISR,URX0_VECTOR)
{
    uint8 c, s;

    URX0IF = 0;  
    c= U0DBUF;
    s= U0CSR;

    if ((s&(CSR_FE|CSR_ERR))==0)    { // Check for Frame and parity errors
        bufPut(&rbRxBuf,&c,1);
    }
}


#ifndef HAL_UART_TX_POLLING
/**************************************************************************//**
* @brief   ISR framework for the USCI A0/B0 transmit component
*
* @param   none
*
* @return  none
*/
HAL_ISR_FUNCTION(uart0TxISR,UTX0_VECTOR)
{
    uint8 c;

    UTX0IF = 0;
    if (bufGet(&rbTxBuf,&c,1)==1) {
        U0DBUF= c;
    }
    // If buffer empty, disable uart TX interrupt
    if( (bufNumBytes(&rbTxBuf)) == 0) {
        IEN2&= ~UTX0IE;
    }
}
#endif
