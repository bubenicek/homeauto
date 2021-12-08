/*******************************************************************************
*  Filename:        prop_regs.h
*  Revised:         $Date: 2014-06-26 16:14:11 +0200 (to, 26 jun 2014) $
*  Revision:        $Revision: 13250 $
*
*  Description:     Header that defines the RAM-based registers on CC2541, 
*                   CC2543, CC2544 and CC2545.
*
*                   There are two kinds of definitions: Standard definitions as 
*                   for the hardware registers and a struct based definition. 
*                   The latter can be used in the debugger (open PRF in the 
*                   watch window)
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


#ifndef __PROP_REG_H
#define __PROP_REG_H
/*******************************************************************************
* INCLUDES
*/
// Include chip specific IO definition file.
#if (chip == 2541)
#include <ioCC2541.h>
#endif
#if (chip == 2543)
#include <ioCC2543.h>
#endif
#if (chip == 2544)
#include <ioCC2544.h>
#endif
#if (chip == 2545)
#include <ioCC2545.h>
#endif


/*******************************************************************************
* CONSTANTS AND DEFINES
*/
#ifdef __IAR_SYSTEMS_ICC__
/* Definition below works only with little-endian registers, and only with a 
*  little-endian compiler (such as IAR) */
#define DXREG(addr)       (((unsigned short volatile __xdata *) addr)[0])
#elif defined __IAR_SYSTEMS_ASM__
#define DXREG(addr)       addr
#endif

#ifndef __IAR_SYSTEMS_ASM__
typedef union {
    unsigned char value;
    struct {
        unsigned char FREQ:7;
        unsigned char SYNTH_ON:1;
    };
} chan_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char MODE:2;
        unsigned char REPEAT:1;
        unsigned char START_CONF:1;
        unsigned char STOP_CONF:2;
        unsigned char TX_ON_CC_CONF:1;
        unsigned char REPEAT_CONF:1;
    };
} task_conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char AUTOFLUSH_IGN:1;
        unsigned char AUTOFLUSH_CRC:1;
        unsigned char AUTOFLUSH_EMPTY:1;
        unsigned char RX_STATUS_CONF:1;
        unsigned char RX_ADDR_CONF:2;
        unsigned char TX_ADDR_CONF:2;
    };
} fifo_conf_t;

typedef union {
    unsigned char value;
    struct {
      unsigned char ADDR_LEN:1;
      unsigned char AGC_EN:1;
      unsigned char START_TONE:1;
      unsigned char :1;
      unsigned char QUICK_TX:4;
    };
} pkt_conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char RXCAP:2;
        unsigned char TXCAP:2;
        unsigned char TXIF:2;
        unsigned char DCOFF:1;
        unsigned char DCWB:1;
    };
} radio_conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char ENA0:1;
        unsigned char ENA1:1;
        unsigned char REUSE:1;
        unsigned char AA:1;
        unsigned char VARLEN:1;
        unsigned char FIXEDSEQ:1;
        unsigned char TXLEN:1;
    };
} conf_t;

typedef union {
    unsigned char value;
    struct {
        unsigned char VALID:1;
        unsigned char SEQ:2;
        unsigned char ACKSEQ:2;
        unsigned char ACK_PAYLOAD_SENT:1;
        unsigned char NEXTACK:1;
    };
} seqstat_t;

typedef union {
    unsigned char value[12];
    struct {
        conf_t CONF;
        unsigned char RXLENGTH;
        unsigned char ADDRESS;
        seqstat_t SEQSTAT;
        unsigned char ACKLENGTH[2];
        unsigned short CRCVAL;
        unsigned char N_TXDONE;
        unsigned char N_RXIGNORED;
        unsigned char N_RXOK;
        unsigned char N_RXNOK;
    };
} address_entry_t;
    
typedef enum {
    TASK_ENDOK = 0,
    TASK_RXTIMEOUT = 1,
    TASK_NOSYNC = 2,
    TASK_NOCC = 3,
    TASK_MAXRT = 4,
    TASK_STOP = 5,
    TASK_ABORT = 6,
    TASK_UNDEF = 15,
    TASKERR_INTERNAL = 255,
    TASKERR_CMD = 254,
    TASKERR_SEM = 253,
    TASKERR_PAR = 252,
    TASKERR_TXFIFO = 251,
    TASKERR_RXFIFO = 250,
    TASKERR_MODUNF = 249
} end_cause_t;


typedef struct {
    chan_t CHAN;
    task_conf_t TASK_CONF;
    fifo_conf_t FIFO_CONF;
    pkt_conf_t PKT_CONF;
    unsigned char CRC_LEN;
    unsigned char RSSI_LIMIT;
    unsigned short RSSI_COUNT;
    unsigned char CRC_INIT[4];
    unsigned char W_INIT;
    unsigned char RETRANS_CNT;
    unsigned short TX_DELAY;
    unsigned short RETRANS_DELAY;
    unsigned short SEARCH_TIME;
    unsigned short RX_TX_TIME;
    unsigned short TX_RX_TIME;
    address_entry_t ADDR_ENTRY[8];
    unsigned char N_TX;
    unsigned char LAST_RSSI;
    signed short LAST_DCOFF[2];
    radio_conf_t RADIO_CONF;
    end_cause_t ENDCAUSE;
} prf_reg_t;

__xdata __no_init volatile prf_reg_t PRF @ 0x6000;
#endif


// The following register should be moved to ioCC254x.h IAR support files when possible. 
#ifndef ACOMPQS
#define ACOMPQS                 XREG(0x61B6)
#endif

#define PRF_CHAN                XREG(0x6000)
#define PRF_TASK_CONF           XREG(0x6001)
#define PRF_FIFO_CONF           XREG(0x6002)
#define PRF_PKT_CONF            XREG(0x6003)
#define PRF_CRC_LEN             XREG(0x6004)
#define PRF_RSSI_LIMIT          XREG(0x6005)
#define PRF_RSSI_COUNT          DXREG(0x6006) // 2 bytes
#define PRF_CRC_INIT            PXREG(0x6008) // 4 bytes
#define PRF_W_INIT              XREG(0x600C)
#define PRF_RETRANS_CNT         XREG(0x600D)
#define PRF_TX_DELAY            DXREG(0x600E) // 2 bytes
#define PRF_RETRANS_DELAY       DXREG(0x6010) // 2 bytes
#define PRF_SEARCH_TIME         DXREG(0x6012) // 2 bytes
#define PRF_RX_TX_TIME          DXREG(0x6014) // 2 bytes
#define PRF_TX_RX_TIME          DXREG(0x6016) // 2 bytes
#define PRF_ADDR_ENTRY0         PXREG(0x6018) // 12 bytes
#define PRF_ADDR_ENTRY1         PXREG(0x6024) // 12 bytes
#define PRF_ADDR_ENTRY2         PXREG(0x6030) // 12 bytes
#define PRF_ADDR_ENTRY3         PXREG(0x603C) // 12 bytes
#define PRF_ADDR_ENTRY4         PXREG(0x6048) // 12 bytes
#define PRF_ADDR_ENTRY5         PXREG(0x6054) // 12 bytes
#define PRF_ADDR_ENTRY6         PXREG(0x6060) // 12 bytes
#define PRF_ADDR_ENTRY7         PXREG(0x606C) // 12 bytes
#define PRF_N_TX                XREG(0x6078)
#define PRF_LAST_RSSI           XREG(0x6079)
#define PRF_LAST_DCOFF          PXREG(0x607A) // 4 bytes
#define PRF_RADIO_CONF          XREG(0x607E)
#define PRF_ENDCAUSE            XREG(0x607F)

// RAM-Based Registers in RAM Page 5
#define PRFX_LAST_FREQEST       XREG(0x6006)
#define PRFX_RSSI_LIM_LOWER     XREG(0x6008)
#define PRFX_RSSI_LIM_UPPER     XREG(0x6009)
#define PRFX_RSSI_DIFF          XREG(0x600A)
#define PRFX_LNAGAIN_SAT        XREG(0x600B)
#define PRFX_TONE_DURATION      XREG(0x600C)  // 2 bytes
#define PRFX_TONE_OFFSET        XREG(0x600E)  // 2 bytes


#define CONF_OFFSET             0
#define RXLENGTH_OFFSET         1
#define ADDRESS_OFFSET          2
#define SEQSTAT_OFFSET          3
#define ACKLENGTH0_OFFSET       4
#define ACKLENGTH1_OFFSET       5
#define CRCVAL_OFFSET           6
#define N_TXDONE_OFFSET         8
#define N_RXIGNORED_OFFSET      9
#define N_RXOK_OFFSET           10
#define N_RXNOK_OFFSET          11

// LLE commands
#define CMD_SHUTDOWN            0x01
#define CMD_DEMOD_TEST          0x02
#define CMD_RX_TEST             0x03
#define CMD_TX_TEST             0x04
#define CMD_TX_FIFO_TEST        0x05
#define CMD_PING                0x06
#define CMD_RX                  0x08
#define CMD_TX                  0x09
#define CMD_TX_ON_CC            0x0A
#define CMD_STOP                0x0B
#define CMD_SEND_EVENT1         0x21
#define CMD_SEND_EVENT2         0x22
#define CMD_FLUSH_ACK0          0x30
#define CMD_FLUSH_ACK1          0x31
#define CMD_FLUSH_ACK2          0x32
#define CMD_FLUSH_ACK3          0x33
#define CMD_FLUSH_ACK4          0x34
#define CMD_FLUSH_ACK5          0x35
#define CMD_FLUSH_ACK6          0x36
#define CMD_FLUSH_ACK7          0x37

// FIFO commands
#define CMD_RXFIFO_RESET        0x81
#define CMD_RXFIFO_DEALLOC      0x82
#define CMD_RXFIFO_RETRY        0x83
#define CMD_RXFIFO_DISCARD      0x84
#define CMD_RXFIFO_COMMIT       0x85
#define CMD_TXFIFO_RESET        0x91
#define CMD_TXFIFO_DEALLOC      0x92
#define CMD_TXFIFO_RETRY        0x93
#define CMD_TXFIFO_DISCARD      0x94
#define CMD_TXFIFO_COMMIT       0x95
#define CMD_FIFO_RESET          0xF1
#define CMD_FIFO_DEALLOC        0xF2
#define CMD_FIFO_RETRY          0xF3
#define CMD_FIFO_DISCARD        0xF4
#define CMD_FIFO_COMMIT         0xF5

#endif