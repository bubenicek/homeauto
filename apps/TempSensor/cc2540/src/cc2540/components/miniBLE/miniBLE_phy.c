/******************************************************************************
*   Filename:       miniBLE_phy.c
*   Revised:        $Date: 2014-07-08 20:44:17 +0200 (ti, 08 jul 2014) $
*   Revision:       $Revision: 126 $
*
*   Description:    miniBLE PHY layer.
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


/*****************************************************************************
* INCLUDES
*/
#if(chip==2541)
#include "ioCC2541.h"
#elif(chip==2543)
#include "ioCC2543.h"
#else
#warning "Device is not supported!"
#error "Chip not supported!"
#endif
#include "miniBLE.h"
#include "miniBLE_phy.h"
#include "miniBLE_defs.h"
#include "prop_regs.h"
#include "string.h"
#include "hal_types.h"
#include "miniBLE_scheduler.h"
#include "power_mode.h"

#if(SMARTRF05EB && DEBUG)
#include "hal_button.h"
#include "hal_led.h"
#include "util_lcd.h"
#include "hal_lcd.h"
#endif

#if(DTM)
// Pseudo-Random 9-bit Data (PSBS9).
const uint8 psbs9Data[] = { 0xFF, 0xC1, 0xFB, 0xE8, 0x4C, 0x90, 0x72, 0x8B,
0xE7, 0xB3, 0x51, 0x89, 0x63, 0xAB, 0x23, 0x23,
0x02, 0x84, 0x18, 0x72, 0xAA, 0x61, 0x2F, 0x3B,
0x51, 0xA8, 0xE5, 0x37, 0x49, 0xFB, 0xC9, 0xCA,
0x0C, 0x18, 0x53, 0x2C, 0xFD };
#endif

volatile uint8 firstActiveChannel   = 0x01;         // First active channel, 37, 38 or 39.
volatile uint8 ActiveChannels       = 0x07;         // Channel Map for active broadcast channels.
volatile uint8 miniBleAdvEventDone  = 1;
volatile uint8 miniBleAdvTxDone     = 1;

#if(SMARTRF05EB)
/*  4 byte variables for counting number advertising events and number of
*   packets sent. If all broadcast channels are active three packets will be
*   transmitted per event. */
volatile uint32 packets_sent      = 0;
volatile uint32 adv_events        = 0;
#endif


/*  When in DTM mode a different RF isr is used, but this will use the same
*   radio settings. */
#if(!DTM)
/******************************************************************************
* @fn       RF interrupt service routine.
*
* @brief    Handle RF interrupt events and clear flags. The isr will also handle
*           the frequency switching between the active broadcast channels and
*           will initiate restart of transmission.
*
* @return   void
*/
HAL_ISR_FUNCTION_SPEED_OPTIMIZE(RF_ISR, RF_VECTOR) {
    unsigned char tx_flag = 0, channel = 0;

    // Read the IRQ flag register.
    unsigned char rfirqf1 = RFIRQF1;
    // Clear RF (general) interrupt flags.
    S1CON = 0;

    // Check for TASKDONE.
    if(rfirqf1 & RFIRQF1_TASKDONE) {
        /* Check if task ends in any other way than OK -< error. Should never happen */
        if(PRF.ENDCAUSE != TASK_ENDOK) {
#if(SMARTRF05EB && DEBUG)
            // Check to see if radio operation is aborted.
            if (PRF.ENDCAUSE == TASK_ABORT) {
                halLcdWriteLines("**Task Aborted**", "  CMD_SHUTDOWN  ", "    ISSUED?     ");
            }
            else {
                // Print for debug.
                halLcdWriteLines("*RF Task Error*", "END CAUSE: ", "");
                halLcdDisplayValue(3, "", (unsigned long)PRF.ENDCAUSE, " ");
            }
            while(1);
#endif
        }

        if(rfirqf1 & RFIRQF1_TXDONE) {
#if(SMARTRF05EB)
            // Increment packet (TX) counter.
            packets_sent++;
#endif
            /* Read out current frequency and change to next active broadcast channel.
             * If There is no more active channels than the current, clear the tx_flag
             * to make sure no transmission is started. Also deallocate the TXFIFO so that
             * it is ready to accept a new payload.
             */
            channel = PRF.CHAN.FREQ;
            switch(channel) {
            case 23 :
                if(ActiveChannels & 0x02) {
                    if(!miniBLEPhySetChannel(CHANNEL_38)) {
                        tx_flag = 1;
                    }
                }
                else if(ActiveChannels & 0x04) {
                    if(!miniBLEPhySetChannel(CHANNEL_39)) {
                        tx_flag = 1;
                    }
                }
                else {
                    tx_flag = 0;
                }
                break;
            case 47 :
                if(ActiveChannels & 0x04) {
                    if(!miniBLEPhySetChannel(CHANNEL_39)) {
                        tx_flag = 1;
                    }
                }
                else {
                    tx_flag = 0;
                }
                break;
            case 101 :
                tx_flag = 0;
                break;
            default :
                tx_flag = 0;
                break;
            }
            // Clear the TXDONE flag in RFIRQF1.
            RFIRQF1 = ~RFIRQF1_TXDONE;
        }
        // Clear the TASKDONE flag in RFIRQF1.
        RFIRQF1 = ~RFIRQF1_TASKDONE;
        // Set to a undefined value in enum type end cause.
        PRF.ENDCAUSE = TASK_UNDEF;
    }

    /*  Set RFRXFRP:= RFRXFSRP in TXFIFO ("read pointer" to same address as
    *   "start read pointer") to resue last packet transmitted to avoid loading
    *   payload into TXFIFO again. REUSE must be set to 1 for this to work,
    *   otherwise radio LLE will deallocate packet in TXFIFO after transmission.*/
    MINIBLE_PHY_CMD(CMD_TXFIFO_RETRY);

    if(tx_flag) {
#if(!TX_INTERVAL)
        /* For interval between each packet the TX_CMD is issued from the scheduler. */
        MINIBLE_PHY_CMD(CMD_TX);    // Start transmitter.
        miniBleAdvTxDone = 0;
#else
        miniBleAdvTxDone = 1;
#endif
    }
    else {
        /*  End of Broadcast event,
        *   schedule next event and set first active channel */

        // Set first active channel.
        miniBLEPhySetChannel(firstActiveChannel);

        /*  Set flag to indicate end of transmission and broadcast event. Device
        *   can now enter sleep. */
        miniBleAdvTxDone = miniBleAdvEventDone = 1;
#if(SMARTRF05EB)
        // Increment broadcast event counter.
        adv_events++;
#endif
    }
    /*  Prohibiting program to enter any power mode by the use of
    *   EnterSleepModeDisableInterruptsOnWakeupUsingFlag. */
    EnterSleepModeFlag = 0;
}
#else
/******************************************************************************
* @brief        Handle RF interrupt events in DTM mode. This routine is designed
*               only for TXDONE interrupt enabled.
*/
HAL_ISR_FUNCTION(RF_DTM_ISR, RF_VECTOR) {
    // Read the IRQ flag register only once not to miss interrupt events.
    unsigned char rfirqf1 = RFIRQF1;

    // Check if TX payload is off-loaded from TXFIFO to modulator.
    if(rfirqf1 & RFIRQF1_TXDONE) {
        /*  The ISR is time critical for issuing a retry TXFIFO command before
        *   T2E1 triggers the next tx command. If TX is started before
        *   CMD_TXFIFO_RETRY is issued the pointers in the TXFIFO will indicate
        *   no packet payload and the radio task will end with TASK_ENDOK if the
        *   radio is set up for REPEATed operation. */
        MINIBLE_PHY_CMD(CMD_TXFIFO_RETRY);  // Reuse payload.
        RFIRQF1 = ~rfirqf1;                 // Clear observed interrupt flags.
    } else {
        /*  For TX issued commands this should never occur unless some unlikely
        *   event occurs. */
        while(1);
    }
    S1CON = 0;      // Clear RF (general) interrupt flags.
}
#endif


/*******************************************************************************
* @fn          	miniBlePhyInit
*
* @brief       	Initialize the radio settings required for miniBLE operation.
*
* @param        void
*
* @return       void
*/
void miniBlePhyInit(void) {
    // Reset radio and clear some of the registers.
    miniBlePhyReset();

    /*  Most of these settings are set to 0 which will already be set in
    *   miniBLEPhyReset, but they are listed here to illustrate some of the key
    *   radio settings for the reader. */
    PRF.TASK_CONF.MODE              = 0;    // Basic mode, fixed length.
    PRF.TASK_CONF.REPEAT            = 0;    // no repeat.
    PRF.TASK_CONF.START_CONF        = 0;    // Start transmit immediately on command.
    PRF.TASK_CONF.STOP_CONF         = 0;    // Don't stop on timer 2 event 2.
    PRF.FIFO_CONF.TX_ADDR_CONF      = 0;    // Read address from PRF.ADDR_ENTRY[0].ADDRESS.
    PRF.PKT_CONF.ADDR_LEN           = 0;    // No address byte.
    PRF.PKT_CONF.AGC_EN             = 0;    // AGC disabled.
    PRF.PKT_CONF.START_TONE         = 0;    // No tone in front of packet.
    PRF.ADDR_ENTRY[0].CONF.REUSE    = 1;    // Reuse packet on same adv event (same payload on all three if active broadcast channels.)

    /*  Register settings that needs to change from default values. If any
    *   register settings are changed due to alterations in the code it will
    *   not be controlled by the miniBLE implementation and the user must take
    *   care to understand and control its impact on the program. */
#if(chip == 2541)
    TXPOWER   = TXPOWER_0_DBM;      // Set default output power: 0dBm.
    TXFILTCFG = 0x03;               // Set Tx filter bandwidth.
    TXCTRL    = 0x19;               // Set DAC Current.
    IVCTRL    = 0x1B;               // Set PA, mixer and DAC bias.
#elif(chip == 2543)
    TXPOWER   = TXPOWER_4_DBM;      // Set default output power: 4 dBm.
    TXFILTCFG = 0x07;               // Set Tx filter bandwidth.
#endif
    MDMCTRL0  = 0x04;               // Set 1 Mbps at 250 kHz deviation.
    MDMCTRL2  = 0x00;               // Syncword transmitted LSB to MSB, 1 leading preamble byte.
    FRMCTRL0  = 0x40;               // Data goes LSB over the air.

    miniBlePhySetPrimarySyncWord(0x8E89BED6, 32);   // Set 32 bit Sync Word.
    MINIBLE_PHY_SET_CRC();                          // Set 3 byte CRC (polynomial 24 bit).
    MINIBLE_PHY_ENABLE_WHITENING();                 // Enable PN7 whitener.
    MINIBLE_PHY_ENABLE_LLE();                       // Enable the radio.
    MINIBLE_PHY_INT_ENABLE_MASK(RFIRQM1_TASKDONE);
    MINIBLE_PHY_INT_ENABLE();                       // Enable RF interrupt.
    return;
}


/******************************************************************************
* @fn          miniBLEPhyReset
*
* @brief       Stop radio operation, reset all RF registers, etc.
*
* @return      void
*/
void miniBlePhyReset(void) {
    // Disable Radio interrupts.
    MINIBLE_PHY_INT_DISABLE();
    uint8 lleStat = LLESTAT;
    /* Check if radio is idle and end any ongoiong operation gracefully if not.*/
    if((LLECTRL != 0) && !(lleStat & LLESTAT_LLE_IDLE)) {
        MINIBLE_PHY_CMD(CMD_STOP);
        /*  A CMD_SHUTDOWN will stop any active tasks immediately, even aborting
        *   ongoing transmission or reception. But CMD_STOP is used here as it
        *   ends the operation more graceully, waiting for ongoing TX/RX
        *   operations to complete. This might increase the time it takse to
        *   set the LLE in reset than comapred to using CMD_SHUTDOWN. */
        while(!(RFIRQF1 & RFIRQF1_TASKDONE));
    }
    MINIBLE_PHY_INT_CLEAR();                // Clear all RF interrupt flags.
    MINIBLE_PHY_CMD(CMD_TXFIFO_RESET);      // Reset TXFIFO.

    /*  Clear radio memory. The RAM registers don't have a default value set and
    *   must be cleared manually. */
    memset((void*)RFCORE_RAM_PAGE, 0, RFCORE_RAM_PAGE_SZ);

    // Set to a undefined value in enum type end cause.
    PRF.ENDCAUSE = TASK_UNDEF;

    // Set RF and ST ISR to highest priority.
    MINIBLE_PHY_SET_PRIORITY();

    // Make sure timer2 events are disabled (No timer 2 events sent to LLE).
    T2EVTCFG = 0x77;
    return;
}


/******************************************************************************
* @fn          	miniBlePhySetSyncWord
*
* @brief       	Set Primary Sync Word.
*
* @param        sw Sync word.
* @param        len Length of sync word in bits.
*
* @return       void
*/
void miniBlePhySetPrimarySyncWord(uint32 sw, uint8 len) {
    SW_CONF = len & 0x1F;
    SW0 = (uint8) sw;
    SW1 = (uint8) (sw>>8);
    SW2 = (uint8) (sw>>16);
    SW3 = (uint8) (sw>>24);
    return;
}


/******************************************************************************
* @fn          	miniBLEPhySetChannel
*
* @brief       	Change broadcast channel.
*
* @note         LLE must be idle while changing frequency.
*
* @param        unsigned char AdvChannel: Advertising channel 37, 38 or 39.
*
* @return       Generic miniBLE function return value.
*/
unsigned char miniBLEPhySetChannel(unsigned char AdvChannel) {

    // Check if radio is idle or else return failure.
    if(RFSTAT & (RFSTAT_TX_ACTIVE | RFSTAT_RX_ACTIVE)) {
        // Radio is in TX/RX, exit with fail.
        return MINIBLE_FAIL_RADIO_ACTIVE;
    }

    /*  Initialization of PN7 whitener in accordance with advertising channel.
    *   PRF_W_INIT should be set to 37, 38, or 39 (0x25, 0x26, or 0x27). */
    switch(AdvChannel) {
    case CHANNEL_37 :
        MINIBLE_PHY_SET_FREQUENCY(23);      // Set frequency to 2402 MHz.
        PRF_W_INIT = 0x25;                  // Init according to BLE channel 37.
        break;
    case CHANNEL_38 :
        MINIBLE_PHY_SET_FREQUENCY(47);      // Set frequency to 2426 MHz.
        PRF_W_INIT = 0x26;                  // Init according to BLE channel 37.
        break;
    case CHANNEL_39 :
        MINIBLE_PHY_SET_FREQUENCY(101);     // Set frequency to 2480 MHz.
        PRF_W_INIT = 0x27;                  // Init according to BLE channel 37.
        break;
    default :
        // Illegal channel in argument.
        return MINIBLE_FAIL_INVALID_ARGUMENT;
        break;
    }
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn          	miniBLEPhySetFrequency
*
* @brief       	Change frequency directly.
*
* @note         LLE must be idle while changing frequency.
*
* @param        frequency Frequency in ( 2379 + frequency ) MHz.
*
* @return       Generic miniBLE function return value.
*/
unsigned char miniBLEPhySetFrequency(unsigned char frequency) {
    // Check if radio is idle or else return failure.
    if(RFSTAT & (RFSTAT_TX_ACTIVE | RFSTAT_RX_ACTIVE)) {
        // Radio is in TX/RX, exit with fail.
        return MINIBLE_FAIL_RADIO_ACTIVE;
    }
    MINIBLE_PHY_SET_FREQUENCY(frequency);
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn          miniBLEPhyLoadPacket
*
* @brief       Load a ADV_NONCONN_IND BLE broadcast packet.
*
* @param       advData Pointer to advertising data buffer.
* @param       len Length of advertising data.
*
* @return      Generic miniBLE function return value.
*/
unsigned char miniBLEPhyLoadPacket(unsigned char *advData, uint8 len) {
           
    // Check for maximum allowed payload size (AdvData allowed size: 0-31 bytes).
    if(len > 27) {
        return MINIBLE_FAIL_INVALID_ARGUMENT;
    } else if (!MINIBLE_PHY_IS_LLE_IDLE()) {
        return MINIBLE_FAIL_RADIO_ACTIVE;
    }

    // BLE header and TXFIFO length parameter (required).
    RFD = len + 12;      // FIFO entry length (not transmitted).

    /*  Advertising channel PDU Header ( 2 Bytes ):
    * ->| PDU Type (4 bits) | RFU (2 bits) | TxAdd (1 bit) | RxAdd (1 bit)  | ---
    *   | Length (6 bits) | RFU (2 bits) |<-
    *
    *   TxAdd in the Flags field indicates whether the advertiser’s address in the AdvA
    *   field is public (TxAdd = 0) or random (TxAdd = 1). */
#if(BDADDR_PUBLIC)
#warning "A valid public address (IEEE) must be loaded in flash because the CC2543 does not have one in the infopage"
    RFD = 0x02;             // ADV_NONCONN_IND.
#else
    // Random address.
    RFD = 0x42;             // ADV_NONCONN_IND + TxAdd=1.
#endif
    RFD = len + 10;         // BLE length byte (transmitted).

    // Advertiser’s public or random device address (required).
    RFD = deviceAddress[0]; // Address (LSB)
    RFD = deviceAddress[1];
    RFD = deviceAddress[2];
    RFD = deviceAddress[3];
    RFD = deviceAddress[4];
    RFD = deviceAddress[5]; // Address (MSB)

/* fix. RBU: flags are set directly in advData buffer
    // Flags.
    RFD = 0x02;         // Length of next Data (3  Byte).
    RFD = 0x01;         // Type flag.
    RFD = 0x06;         // BR/EDR Not Supported.
    //RFD = len;          // Length of next Data.
*/
    
    // AdvData/payload (optional).
    for (uint8 i = 0; i < len; i++) {
        RFD = advData[i];
    }

    /*  Optimization tip:
    *   To reload the payload only and skip loading in the mandatory header,
    *   address, etc.. the payload can be loaded directly into the TXFIFO using
    *   a xdata pointer to the TXFIFO address RFTXFSRP (0x61D7) + 15.
    *   A packet of equal length and format must already be loaded in TXFIFO
    *   and PRF.ADDR_ENTRY[0].CONF.REUSE must be set to 1 for this to work. */

    // Packet successfully loaded in to TXFIFO.
    return MINIBLE_SUCCESS;
}


#if(DTM)
/******************************************************************************
******************* Direct Test Mode (DTM) related functions ******************
******************************************************************************/

/******************************************************************************
* @fn          	miniBLEPhyLoadDTMPacket
*
* @brief       	Load a test packet for use in DTM mode.
*
* @param        payloadType Type of test packet.
* @param        payloadLen Length of payload.
*
* @return       SUCCESS:                Channel changed successfully.
*               INVALID_PARAMETER:      An illegal argument was supplied.
*               FAIL_RADIO_ACTIVE:      Radio is in TX/RX.
*/
uint8 miniBLEPhyLoadDTMPacket(uint8 payloadType, uint8 payloadLen) {
    /*  The payload length is for the nanoRisc to process the packet, so it must
    *   include the header and the packet payload length byte. This byte is not
    *   transmitted. */
    MINIBLE_PHY_WRITE_BYTE_VAL( payloadLen+2U );    // FIFO packet length.
    MINIBLE_PHY_WRITE_BYTE_VAL( payloadType );      // Header - type of payload.
    MINIBLE_PHY_WRITE_BYTE_VAL( payloadLen );       // Payload length.

    // Enter selected payload.
    switch( payloadType ) {
    case DIRECT_TEST_PAYLOAD_PRBS9:         // Pseudo-Random 9-bit data
        MINIBLE_PHY_WRITE_BYTES( psbs9Data, payloadLen );
        break;
    case DIRECT_TEST_PAYLOAD_0xF0 :         // 0xF0: '00001111'
        for (uint8 i=0; i<payloadLen; i++) {
            MINIBLE_PHY_WRITE_BYTE_VAL( 0xF0 );
        }
        break;
    case DIRECT_TEST_PAYLOAD_0xAA :         // 0xAA: '01010101'
        for (uint8 i=0; i<payloadLen; i++) {
            MINIBLE_PHY_WRITE_BYTE_VAL( 0xAA );
        }
        break;
    default:
        return( STATUS_ERROR_BAD_PARAMETER );
    }
    return MINIBLE_SUCCESS;
}
#endif