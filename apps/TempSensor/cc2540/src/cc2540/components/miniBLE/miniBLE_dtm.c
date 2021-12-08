/******************************************************************************
*   Filename:       miniBLE_dtm.c
*   Revised:        $Date: 2014-07-08 20:44:17 +0200 (ti, 08 jul 2014) $
*   Revision:       $Revision: 126 $
*
*   Description:    Direct Test Mode (DTM) implementation for CC2543.
*
*   Note:           Currently only support 2-wire UART at 115200 bps
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
******************************************************************************/
#include <ioCC254x_bitdef.h>
#if(chip==2541)
#include "ioCC2541.h"
#elif(chip==2543)
#include "ioCC2543.h"
#else
#warning "Device is not supported!"
#endif
#include "miniBLE_phy.h"
#include "miniBLE_dtm.h"
#include "hal_timer2.h"
#include "miniBLE_defs.h"
#include "hal_board.h"
#include <hal_types.h>
#include <hal_wait.h>
#include "prop_regs.h"
#include "hal_int.h"
#include "string.h"
#include "dma.h"
#if(SMARTRF05EB)
#include "hal_board.h"
#include "hal_button.h"
#include "hal_led.h"
#include "util_lcd.h"
#endif


/******************************************************************************
* DEFINES
*/
// Size of allocated UART RX/TX buffer
#define SIZE_OF_UART_RX_BUFFER                2 // Enough for 2-wire DTM
#define SIZE_OF_UART_TX_BUFFER                SIZE_OF_UART_RX_BUFFER

// Baudrate = 115200 bps (U0BAUD.BAUD_M = 216, U0GCR.BAUD_E = 11), given 32 MHz system clock
#define UART_BAUD_M                           216
#define UART_BAUD_E                           11

#define CMD_SUCCESS                           0
#define CMD_FAILURE                           1


/******************************************************************************
* LOCAL VARIABLES
*/
// Buffers for UART RX/TX.
static uint8 __xdata uartRxBuffer[SIZE_OF_UART_RX_BUFFER];
static uint8 __xdata uartTxBuffer[SIZE_OF_UART_TX_BUFFER];
static uint16 __xdata uartTxIndex;
static uint16 __xdata uartRxIndex;

// Variable for UART packet monitoring.
static uint8 __xdata uartPktReceived = 0;

// Flag to indicate end of test.
volatile unsigned char testEnd = 0;


/******************************************************************************
* TYPE DEFS
*/
typedef uint8 cmdStatus_t;


/******************************************************************************
* LOCAL FUNCTIONS PROTOTYPES
*/
static void uart0StartRxForIsr(void);
static void uart0StartTxForIsr(void);
static void halUARTinit(void);
static void cmdCompleteEvent( uint8 command, uint8 status );
static cmdStatus_t miniBLETransmitterTestCmd( uint8 txFreq, uint8 dataLength, uint8 payloadType );



/***********************************************************************************
* @fn          UART0_TX_ISR
*
* @brief       Function which completes the UART transmit session, that is it
*              sends the rest of the UART0 packet.
*
* @param       void
*/
#pragma vector = UTX0_VECTOR
__interrupt void UART0_TX_ISR(void) {
    // Clear UART0 TX Interrupt Flag (IRCON2.UTX0IF = 0).
    UTX0IF = 0;

    // Send next UART byte.
    U0DBUF = uartTxBuffer[uartTxIndex++];

    // If no UART byte left to transmit, stop this UART TX session
    if (uartTxIndex >= SIZE_OF_UART_TX_BUFFER)
    {
        /*  In order to start another UART TX session the application just
        *   needs to prepare the source buffer, and trigger the TX isr. */
        uartTxIndex = 0;
        IEN2 &= ~IEN2_UTX0IE;
    } else {
        // Trigger TX interrupt to start transmitting next byte.
        UTX0IF = 1;
    }
}


/***********************************************************************************
* @fn          UART0_RX_ISR
*
* @brief       Function which completes the UART receive session for DTM mode only!
*
* @param       void
*
*/
#pragma vector = URX0_VECTOR
__interrupt void UART0_RX_ISR(void)
{
  // Clear UART0 RX Interrupt Flag (TCON.URX0IF = 0).
  URX0IF = 0;

  // Read UART0 RX buffer.
  uartRxBuffer[uartRxIndex++] = U0DBUF;

  // If all UART data received, stop this UART RX session.
  if (uartRxIndex >= SIZE_OF_UART_RX_BUFFER)
  {
    uartRxIndex = 0;
    uartPktReceived = 1;
  }
}


/******************************************************************************
* @fn       halStartDTM
*
* @brief    Start the Direct Test Mode (DTM) program.
*
* @return   void
*/
void miniBleDtmStart(void) {
    uint8 tmp = 0, cmd = 0, txFreq = 0, dataLength = 0, payloadType = 0;
    cmdStatus_t status;

    //Initiate UART communication
    halUARTinit();
    uart0StartRxForIsr();

    while(1){
        if(uartPktReceived) {
            // Fetch Bitwise Data from first byte
            tmp = uartRxBuffer[0];
            txFreq = tmp & 0x3F;
            cmd = ((tmp >> 6) & 0x03);

            // Fetch Bitwise Data from second byte
            tmp = uartRxBuffer[1];
            dataLength = ((tmp >> 2) & 0x3F);
            payloadType = tmp & 0x03;

            // EIV: Do more checks like see if command is already running etc....

            //Check Packet
            switch(cmd) {
            case LE_RESET_CMD :
                /*  This section could be extended to reset all relevant
                *   settings/configurations */
                cmdCompleteEvent( LE_RESET_CMD, CMD_SUCCESS );
                break;
            case LE_RECEIVER_TEST_CMD :
                /* Not supported, send command failure */
                cmdCompleteEvent( LE_RECEIVER_TEST_CMD, CMD_FAILURE );
                break;
            case LE_TRANSMITTER_TEST_CMD :
                status = miniBLETransmitterTestCmd( txFreq, dataLength, payloadType );
                cmdCompleteEvent( LE_TRANSMITTER_TEST_CMD, status );
                break;
            case LE_END_TEST_CMD :
                // Reset radio and clear some of the registers.
                miniBlePhyReset();
                cmdCompleteEvent( LE_END_TEST_CMD, CMD_SUCCESS );
                break;
            default :
                // Illegal command
                cmdCompleteEvent( LE_END_TEST_CMD, CMD_FAILURE );
                break;
            }
            // Reset flags and parameters.
            uart0StartRxForIsr();
        }
    }
}


/******************************************************************************
* @fn          	miniBLETransmitterTestCmd
*
* @brief        Setupt automatic transmittion of DTM test packets. T2 is used
*               to repeat transmission every 1.25 ms
*
* @param        txFreq  TX Frequency.
* @param        payloadLen Length of test packet payload.
* @param        payloadType Type of test packet to use in test.
*
* @return       cmdStatus_t
*/
cmdStatus_t miniBLETransmitterTestCmd(uint8 txFreq, uint8 payloadLen, uint8 payloadType ) {
    // Verify that input parameters are valid
    if ( (txFreq >= DIRECT_TEST_NUM_RF_CHANS)           ||
        (payloadLen > DIRECT_TEST_MAX_PAYLOAD_LEN)      ||
            ((payloadType != DIRECT_TEST_PAYLOAD_PRBS9) &&
             (payloadType != DIRECT_TEST_PAYLOAD_0xF0)  &&
                 (payloadType != DIRECT_TEST_PAYLOAD_0xAA)) )  {
                     return( STATUS_ERROR_BAD_PARAMETER );
                 }

    // Initialize the radio settings.
    miniBlePhyInit();

    // Override some miniBLE standard settings for DTM mode.
    FRMCTRL0 = 0x41;                        // Data goes MSB over the air.
    PRF.TASK_CONF.REPEAT            = 1;    // Repeat.
    PRF.TASK_CONF.START_CONF        = 1;    // Start each receive/transmit on Timer 2 event 1.
    PRF.ADDR_ENTRY[0].CONF.REUSE    = 1;    // Reuse same payload for every test packet.

    /*  LE test packets shall have ‘10010100100000100110111010001110’ in
    *   transmission order as their synchronization word (0x71764129 ).*/
    miniBlePhySetPrimarySyncWord(0x71764129, 32);

    /*  The CRC shift register shall be preset with 0x555555 for every LE test
    *   packet. */
    MINIBLE_PHY_SET_CRC();

    // Disable both Whiteners
    BSP_MODE &= ~(BV(0) | BV(1));

    // Enter payload.
    miniBLEPhyLoadDTMPacket(payloadType, payloadLen);

    /* Reset Timer2, set base period and set T2E2 to overflow period. */
    halTimer2Reset();
    halTimer2SetBasePeriod(0x09C4);
    T2EVTCFG = 0x73;

    /*  Set frequency, multiply txFreq with 2 to achieve 2 MHz channel spacing
    *   and add the offset from 0 = 2379 MHz. */
    miniBLEPhySetFrequency((txFreq << 1) + 23);

    // Enable the radio.
    MINIBLE_PHY_ENABLE_LLE();

    // Set RF isr top priority.
    IP0 = IP1 = IP0_RFERR_RF_DMA;

    /*  Enable TXDONE interrupt as we need to run the TXFIFORETRY command before
    *   the next scheduled transmission is triggered. */
    MINIBLE_PHY_INT_CLEAR_MASK();
    MINIBLE_PHY_INT_ENABLE_MASK(RFIRQM1_TXDONE);

    // Enable global interrupt.
    HAL_INT_ON();

    // Set up timer2 for periodic transmission of packets ( 1.25 ms interval ).
    halTimer2UpdateOverflowCompareValue(2,0);

    // Start Timer2 tick.
    halTimer2Start(0);

    // Start transmitter.
    MINIBLE_PHY_CMD(CMD_TX);

    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn           setupDTMTestPort
*
* @brief        Set up UART test port for DTM.
*
* @return       void
*/
void miniBleDtmSetupTestPort( void ) {
#if(SMARTRF05EB)
    /***************************************************************************
    * UART I/O ports on SmartRF05EB (UART0 Alternative 1)
    * RX     : P0_2
    * TX     : P0_3
    * CT/CTS : P0_4 ( Tester RTS )
    * RT/RTS : P0_5 ( Tester CTS )
    */
    P0SEL &= (~BV(4) & ~BV(5)); // Set to GPIO
    P0DIR &= ~BV(4);            // input; this is Tester's RTS
    P0DIR |=  BV(5);            // output; this is Tester's CTS
    P0    |=  BV(5);            // de-assert Tester's CTS
    P0INP &= ~BV(4);            // pull-up/pull-down depending on P2INP
    P2INP &= ~BV(5);            // pull-up
#elif(WEATHER_STATION_BOARD)
    /***************************************************************************
    * UART I/O ports on WEATHER STATION (UART0 Alternative 2)
    * RX     : P1_3
    * TX     : P1_4
    * CT/CTS : P1_1 ( Tester RTS ) ( NOT USED )
    * RT/RTS : P1_2 ( Tester CTS ) ( NOT USED )
    */
    P1SEL &= (~BV(1) & ~BV(2)); // Set to GPIO
    P1DIR &= ~BV(1);            // input; this is Tester's RTS
    P1DIR |=  BV(2);            // output; this is Tester's CTS
    P1    |=  BV(2);            // de-assert Tester's CTS
    // P1.1 is a high drive pin and does not have pull capability.
    // P1.2 is a high drive pin and does not have pull capability.
#else
    while(1);
#endif
    return;
}


/******************************************************************************
* LOCAL FUNCTIONS
*/
/******************************************************************************
* @fn           halUARTinit
*
* @brief        Initialize UART interface for DTM mode.
*
* @return      void
*/
void halUARTinit(void) {

#if(SMARTRF05EB)
    // Configure USART0 for Alternative 1 => Port P0 (PERCFG.U0CFG = 0).
    PERCFG = (PERCFG & ~PERCFG_U0CFG) | PERCFG_U0CFG_ALT1;
    // Set pins 2, 3 and 5 as peripheral I/O and pin 4 as GPIO output.
    P0SEL |= BIT5 | BIT3 | BIT2;
#elif(WEATHER_STATION_BOARD)
    // Configure USART0 for Alternative 2 => Port P1 (PERCFG.U0CFG = b01).
    PERCFG = (PERCFG & ~PERCFG_U0CFG) | PERCFG_U0CFG_ALT2;
    // Set pins P1.3, P1.4 and P1.2 as peripheral I/O and pin P1.1 as GPIO output.
    P1SEL |= BIT4 | BIT3 | BIT2;
#endif

#if (chip==2541)
    // Give priority to USART 0 over Timer 1 for port 0 pins.
    P2DIR &= P2DIR_PRIP0_USART0;
#elif (chip==2543)
    // Give priority to USART 0 over Timer 1 for port 0 pins.
    PPRI &= ~PPRI_PRI0P0;
#endif

    // Initialise bitrate = 115200 bps.
    U0BAUD = UART_BAUD_M;
    U0GCR = (U0GCR&~U0GCR_BAUD_E) | UART_BAUD_E;

    // Initialise UART protocol (start/stop bit, data bits, parity, etc.):
    // USART mode = UART (U0CSR.MODE = 1).
    U0CSR |= U0CSR_MODE;

    // Start bit level = low => Idle level = high  (U0UCR.START = 0).
    U0UCR &= ~U0UCR_START;

    // Stop bit level = high (U0UCR.STOP = 1).
    U0UCR |= U0UCR_STOP;

    // Number of stop bits = 1 (U0UCR.SPB = 0).
    U0UCR &= ~U0UCR_SPB;

    // Parity = disabled (U0UCR.PARITY = 0).
    U0UCR &= ~U0UCR_PARITY;

    // 9-bit data enable = 8 bits transfer (U0UCR.BIT9 = 0).
    U0UCR &= ~U0UCR_BIT9;

    // Level of bit 9 = 0 (U0UCR.D9 = 0), used when U0UCR.BIT9 = 1.
    // Level of bit 9 = 1 (U0UCR.D9 = 1), used when U0UCR.BIT9 = 1.
    // Parity = Even (U0UCR.D9 = 0), used when U0UCR.PARITY = 1.
    // Parity = Odd (U0UCR.D9 = 1), used when U0UCR.PARITY = 1.
    // Parity bit not needed for DTM....
    // U0UCR &= ~U0UCR_D9;

    // Flow control = disabled (U0UCR.FLOW = 0).
    U0UCR &= ~U0UCR_FLOW;

    // Bit order = LSB first (U0GCR.ORDER = 0).
    U0GCR &= ~U0GCR_ORDER;
}


/***********************************************************************************
* @fn          uart0StartTxForIsr
*
* @brief       Function which starts/initiates the transmission of byte sequence
*              on UART0.
*
* @param       void
*
* @return      void
*/
void uart0StartTxForIsr(void) {
  // Initialize the UART TX buffer index.
  uartTxIndex = 0;

  // Clear any pending UART TX Interrupt Flag (IRCON2.UTXxIF = 0, UxCSR.TX_BYTE = 0).
  UTX0IF = 0;
  U0CSR &= ~U0CSR_TX_BYTE;

  // Enable global interrupt (IEN0.EA = 1) and UART TX Interrupt (IEN2.UTXxIE = 1).
  IEN2 |= IEN2_UTX0IE;
  EA = 1;

  // Trigger TX interrupt to start transmitting.
  UTX0IF = 1;
}


/******************************************************************************
* @fn          uart0StartRxForIsr
*
* @brief       Function which starts/initiates the receive of a byte sequence
*              on UART0.
*
* @param       void
*
* @return      void
*/
void uart0StartRxForIsr(void) {
    // Ready for new packet.
    uartPktReceived = 0;

    // Initialize the UART RX buffer index.
    uartRxIndex = 0;

    // Clear any pending UART RX Interrupt Flag (TCON.URXxIF = 0, UxCSR.RX_BYTE = 0).
    URX0IF = 0;

    // Enable UART RX (UxCSR.RE = 1).
    U0CSR |= U0CSR_RE;

    // Enable global Interrupt (IEN0.EA = 1) and UART RX Interrupt (IEN0.URXxIE = 1).
    URX0IE = 1;
    EA = 1;
}


/******************************************************************************
* @fn          cmdCompleteEvent
*
* @brief       Returns status to BLE tester through 2-wire UART.
*
* @param       void
*
* @return      void
*/
void cmdCompleteEvent( uint8 cmd, uint8 status ) {
    uartTxBuffer[0] =0x00;
    uartTxBuffer[1] =status;
    uart0StartTxForIsr(); //Send status
}