/******************************************************************************
*   Filename:       miniBLE_phy.h
*   Revised:        $Date: 2014-07-08 20:44:17 +0200 (ti, 08 jul 2014) $
*   Revision:       $Revision: 126 $
*
*   Description:    header file for miniBLE PHY layer.
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


#ifndef MINIBLE_PHY_H
#define MINIBLE_PHY_H
/******************************************************************************
 * INCLUDES
 */
#if(chip==2543)
#include "ioCC2543.h"
#else
#warning "Device is not supported!"
#endif
#include "prop_regs.h"
#include "ioCC254x_bitdef.h"
#include "hal_types.h"
#include "hal_defs.h"


/******************************************************************************
 * MACROS
 */
#define MINIBLE_PHY_CMD(cmd)        st(while(RFST != 0); RFST = (cmd);)
#define MINIBLE_PHY_RESET_LLE()     LLECTRL &= ~LLECTRL_LLE_EN
#define MINIBLE_PHY_ENABLE_LLE()    LLECTRL |= LLECTRL_LLE_EN
#define MINIBLE_PHY_WRITE_BYTES( data, len )                            \
 st({                                                                   \
    uint8 z;                                                            \
    for (z=0; z<(len); z++) { RFD = (data)[z]; }                        \
  })
#define MINIBLE_PHY_WRITE_BYTE_VAL( value ) (RFD = (value))
#define MINIBLE_PHY_READ_BYTES( ptr, len )                              \
  st( uint8 z;                                                          \
      if ( (ptr) == NULL )                                              \
      {                                                                 \
        for (z=0; z<(len); z++)                                         \
        { RFD; }                                                        \
      }                                                                 \
      else                                                              \
      {                                                                 \
        for (z=0; z<(len); z++)                                         \
        { (ptr)[z] = RFD; }                                             \
      }                                                                 \
    )
#define MINIBLE_PHY_READ_BYTE_VAL()             (RFD)
#define MINIBLE_PHY_SET_FREQUENCY(frequency)    (PRF.CHAN.FREQ=frequency)
#define MINIBLE_PHY_ENABLE_WHITENING()                                  \
  st(                                                                   \
    BSP_MODE = 0x01;                                                    \
    PRF.W_INIT = 0x80;                                                  \
    )
#define MINIBLE_PHY_SET_CRC()                                           \
  st(                                                                   \
    PRF_CRC_LEN = 0x03;                                                 \
    PRF_CRC_INIT[0] = 0x00;                                             \
    PRF_CRC_INIT[1] = 0x55;                                             \
    PRF_CRC_INIT[2] = 0x55;                                             \
    PRF_CRC_INIT[3] = 0x55;                                             \
    BSP_P0 = 0x00;                                                      \
    BSP_P1 = 0x5B;                                                      \
    BSP_P2 = 0x06;                                                      \
    BSP_P3 = 0x00;                                                      \
    )
#define MINIBLE_PHY_IS_LLE_IDLE()               (LLESTAT&0x04)
#define MINIBLE_PHY_SET_OUTPUT_POWER(txpower)   (TXPOWER = txpower)
#define MINIBLE_PHY_WAIT_FOR_TASKDONE()         while(!(RFIRQF1 & RFIRQF1_TASKDONE))
#define MINIBLE_PHY_INT_CLEAR()                 st(RFIRQF1 = 0; S1CON = 0;)
#define MINIBLE_PHY_INT_ENABLE()                (IEN2 |= IEN2_RFIE)
#define MINIBLE_PHY_INT_DISABLE()               (IEN2 &= ~IEN2_RFIE)
#define MINIBLE_PHY_INT_ENABLE_MASK(mask)       (RFIRQM1 |= mask)
#define MINIBLE_PHY_INT_DISABLE_MASK(mask)      (RFIRQM1 &= ~mask)
#define MINIBLE_PHY_INT_CLEAR_MASK()            (RFIRQM1 = 0)


/*  Set interrupt routine priority at level 3 (max) for RF ISR and level 2 for
*   the ST ISR (Scheduler). No other interrupt should be higher than 1 to avoid
*   interrupting either the scheduler or radio event handling */
#define MINIBLE_PHY_SET_PRIORITY()                      \
    st(                                                 \
        IP0 = IP1 = IP1_RFERR_RF_DMA;                   \
        IP1 = IP1_ST_P0INT_WDT;                         \
      )

#define MINIBLE_PHY_GET_CHIP_ID()           (*(P_INFOPAGE+3))


/******************************************************************************
* DEFINES
*/

// BLE broadcast channels.
#define BLE_BROADCAST_CHANNEL_37 23     // 2402 MHz
#define BLE_BROADCAST_CHANNEL_38 47     // 2426 MHz
#define BLE_BROADCAST_CHANNEL_39 101    // 2480 MHz

/* Definitions related to RF phy */

// Definitions for address configurations.
#define ADDR_CONF_RX_AUTO_SW1       0x19
#define ADDR_CONF_RX_AUTO_SW2       0x1A
#define ADDR_CONF_RX_BASIC_SW1      0x01
#define ADDR_CONF_RX_BASIC_SW2      0x02
#define ADDR_CONF_TX_AUTO           0x18
#define ADDR_CONF_TX_BASIC          0x00

// Allowed TXPOWER register values (BT 4.1 spec allows from 10 dBm to -20 dBm).
#if (chip == 2541)
#define TXPOWER_0_DBM           0xE1
#define TXPOWER_MINUS_2_DBM     0xD1
#define TXPOWER_MINUS_4_DBM     0xC1
#define TXPOWER_MINUS_6_DBM     0xB1
#define TXPOWER_MINUS_8_DBM     0xA1
#define TXPOWER_MINUS_10_DBM    0x91
#define TXPOWER_MINUS_12_DBM    0x81
#define TXPOWER_MINUS_14_DBM    0x71
#define TXPOWER_MINUS_16_DBM    0x61
#define TXPOWER_MINUS_18_DBM    0x51
#define TXPOWER_MINUS_20_DBM    0x41
#elif(chip == 2543)
#define TXPOWER_4_DBM           0xE5
#define TXPOWER_0_DBM           0xA5
#define TXPOWER_MINUS_6_DBM     0x65
#define TXPOWER_MINUS_12_DBM    0x45
#define TXPOWER_MINUS_15_DBM    0x25
#define TXPOWER_MINUS_20_DBM    0x05
#endif

#if (chip == 2543)
// Uncalibrated accuracy of 4 dB. ( Valid range: 0 to 64).
#define RSSI_OFFSET_HIGH_GAIN       99
#define RSSI_OFFSET_LOW_GAIN        79
#elif (chip == 2541)
#define RSSI_OFFSET_HIGH_GAIN       107
#define RSSI_OFFSET_LOW_GAIN        98
#define RSSI_OFFSET_AGC_HIGH_GAIN   99
#define RSSI_OFFSET_AGC_LOW_GAIN    79
#endif

// Bitmask for radio register RFIRQF1.
#define RFIRQF1_PINGRSP             0x80
#define RFIRQF1_TASKDONE            0x40
#define RFIRQF1_TXDONE              0x20
#define RFIRQF1_RXEMPTY             0x10
#define RFIRQF1_RXIGNORED           0x08
#define RFIRQF1_RXNOK               0x04
#define RFIRQF1_TXFLUSHED           0x02
#define RFIRQF1_RXOK                0x01

// RFIRQM1 (0x6182) – RF Interrupt Masks
#define RFIRQM1_PINGRSP             0x80
#define RFIRQM1_TASKDONE            0x40
#define RFIRQM1_TXDONE              0x20
#define RFIRQM1_RXEMPTY             0x10
#define RFIRQM1_RXIGNORED           0x08
#define RFIRQM1_RXNOK               0x04
#define RFIRQM1_TXFLUSHED           0x02
#define RFIRQM1_RXOK                0x01
#define RFIRQM1_ALL                 0xFF

// Bitmask for radio register RFSTAT.
#define RFSTAT_MOD_UNDERFLOW        0x80
#define RFSTAT_DEM_STATUS           0x60
#define RFSTAT_SFD                  0x10
#define RFSTAT_CAL_RUNNING          0x08
#define RFSTAT_LOCK_STATUS          0x04
#define RFSTAT_TX_ACTIVE            0x02
#define RFSTAT_RX_ACTIVE            0x01

// Bitmask for radio register LLESTAT.
#define LLESTAT_AGC_LOWGAIN         0x10
#define LLESTAT_WAIT_T2E1           0x08
#define LLESTAT_LLE_IDLE            0x04
#define LLESTAT_SYNC_SEARCH         0x02
#define LLESTAT_VCO_ON              0x01

// Bitmask for radio register SW_CONF
#define SW_CONF_DUAL_RX             0x80
#define SW_CONF_SW_RX               0x20
#define SW_CONF_SW_LEN              0x1F

// Bitmask for radio register MDMCTRL1
#define MDMCTRL1_FOC_MODE           0xC0
#define MDMCTRL1_CORR_THR           0x1F

// Bitmask for radio register RFFSTATUS
#define RFFSTATUS_TXAVAIL           0x80
#define RFFSTATUS_TXFEMPTY          0x40
#define RFFSTATUS_TXDTHEX           0x20
#define RFFSTATUS_TXFFULL           0x10
#define RFFSTATUS_RXAVAIL           0x08
#define RFFSTATUS_RXFEMPTY          0x04
#define RFFSTATUS_RXDTHEX           0x02
#define RFFSTATUS_RXFFULL           0x01

// Bitmask for radio register MDMTEST0
#define MDMTEST0_RSSI_ACC           0xE0
#define MDMTEST0_DC_BLOCK_LENGTH    0x0C
#define MDMTEST0_DC_BLOCK_MODE      0x03

// IP0 (0xA9) – Interrupt Priority 0
#define IP0_ST_P0INT_WDT            0x20
#define IP0_ENC_T4_P1INT            0x10
#define IP0_T3_I2C                  0x08
#define IP0_URX0_T2_UTX0            0x04
#define IP0_ADC_T1_P2INT_USB        0x02
#define IP0_RFERR_RF_DMA            0x01

// IP1 (0xB9) – Interrupt Priority 1
#define IP1_ST_P0INT_WDT            0x20
#define IP1_ENC_T4_P1INT            0x10
#define IP1_T3_I2C                  0x08
#define IP1_URX0_T2_UTX0            0x04
#define IP1_ADC_T1_P2INT_USB        0x02
#define IP1_RFERR_RF_DMA            0x01

// LLECTRL (0x61B1) – LLE Control
#define LLECTRL_LLE_MODE_SEL        0x06
#define LLECTRL_LLE_EN              0x01

// RFC_OBS_CTRL0 (0x61AE) – RF Observation Mux Control 0
#define RFC_OBS_POL0                0x40
#define RFC_OBS_MUX0                0x3F
    #define RFC_SNIFF_DATA          0x07
    #define RFC_SNIFF_CLK           0x08
    #define TX_ACTIVE               0x09
    #define RX_ACTIVE               0x0A
    #define VCO_ON                  0x0B
    #define SYNCH_SEARCH            0x0C
    #define LLE_IDLE                0x0D
    #define WAIT_T2E1               0x0E
    #define AGC_LOWGAIN             0x0F
    #define FSC_LOCK                0x13
    #define PA_PD                   0x1B
    #define LNAMIX_PD               0x2C
    #define DEMO_SYNC_FOUND         0x30
    #define MOD_SYNC_SENT           0x31

// RSSI accuracy setting definitions
#define ONE_5_33_US_AVERAGE         0x00
#define TWO_5_33_US_AVERAGE         0x01
#define FOUR_5_33_US_AVERAGE        0x03
#define ONE_21_3_US_AVERAGE         0x04
#define TWO_21_3_US_AVERAGE         0x05
#define FOUR_21_3_US_AVERAGE        0x07

// Gain modes
#define HAL_RF_GAIN_LOW                 0
#define HAL_RF_GAIN_HIGH                1

/* Definitions related to Direct Test Mode (DTM). */
#if(DTM)
#define LE_RESET_CMD                    0
#define LE_RECEIVER_TEST_CMD            1 // Not part of miniBLE
#define LE_TRANSMITTER_TEST_CMD         2
#define LE_END_TEST_CMD                 3
#define LE_TEST_STATUS_EVT              0
#define LE_PACKET_REPORTING_EVT         1
#define DIRECT_TEST_NUM_RF_CHANS        40
#define DIRECT_TEST_MAX_PAYLOAD_LEN     37

// Payload types used in payload for DTM LE_Transmitter_Test
#define DIRECT_TEST_PAYLOAD_PRBS9             0
#define DIRECT_TEST_PAYLOAD_0xF0              1
#define DIRECT_TEST_PAYLOAD_0xAA              2
#define DIRECT_TEST_PAYLOAD_VENDOR            3

#define DIRECT_TEST_SYNCH_WORD                0x71764129
#define STATUS_ERROR_BAD_PARAMETER            0x12
#endif


/******************************************************************************
* GLOBAL VARIABLES
*/
// Channel Map for active broadcast channels.
extern volatile uint8 ActiveChannels;
// First active channel, 37, 38 or 39.
extern volatile uint8 firstActiveChannel;
extern volatile uint8 miniBleAdvEventDone;
extern volatile uint8 miniBleAdvTxDone;
#if(SMARTRF05EB)
extern volatile uint32 packets_sent;
extern volatile uint32 adv_events;
#endif


/******************************************************************************
* GLOBAL FUNCTIONS
*/
void miniBlePhyReset(void);
void miniBlePhyInit(void);
void miniBlePhySetPrimarySyncWord(uint32 sw, uint8 len);
uint8 miniBLEPhySetFrequency(unsigned char frequency);
uint8 miniBLEPhySetChannel(unsigned char AdvChannel);
uint8 miniBLEPhyLoadPacket(unsigned char *advData, uint8 len);

#if(DTM)
/* DTM realated functions. */
uint8 miniBLEPhyLoadDTMPacket(uint8 payloadType, uint8 payloadLen);
#endif
#endif