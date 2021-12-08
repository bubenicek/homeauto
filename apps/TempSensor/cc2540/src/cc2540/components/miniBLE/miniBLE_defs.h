/******************************************************************************
*   Filename:       miniBLE_defs.h
*   Revised:        $Date: 2014-06-26 12:57:28 +0200 (to, 26 jun 2014) $
*   Revision:       $Revision: 122 $
*
*   Description:    General definitions used in all miniBLE layers.
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

#ifndef MINIBLE_DEFS_H
#define MINIBLE_DEFS_H
/******************************************************************************
* INCLUDES
*/

/******************************************************************************
* DEFINES
*/

/** Device address type 1 = static (generated from production ID)  2 = setted from FW_PARAM_DEVICE_ADDRESS */
#define FW_PARAM_DEVICE_ADDRESS_TYPE    DEVICE_ADDRESS_STATIC


/*** Defines related to scheduler ***/

// Pseudo random delay on advertismentes as given in BLE spec.
#define PSEUDO_ADV_DELAY    1

/*** miniBLE Generic Status Return Values ***/
#define MINIBLE_SUCCESS                     0
#define MINIBLE_SUCCESS_PENDING             1
#define MINIBLE_FAIL                        2
#define MINIBLE_FAIL_INVALID_ARGUMENT       3
#define MINIBLE_FAIL_BUFFER_FULL            5
#define MINIBLE_FAIL_RADIO_ACTIVE           6
#define MINIBLE_FAIL_ADVERTISEMENT          7
#define MINIBLE_FAIL_ALL_EVT_DISABLED       8

#define ON                    1
#define OFF                   0
#define SCHEDULE_NOW          1
#define SCHEDULE_LATER        0

// Task Events.
typedef enum {
    ADV = 0,
    DAQ,
    CAL,
    WAIT
} miniBleEvent_t;

/*  miniBLE scheduler tasks:
*   These are the tasks that will be automatically handled by the scheduler. */
enum {
TASK_ADV = 0,
TASK_DAQ,
TASK_CAL,
TASK_WAIT,
NUM_TASKS
};

// Packet Lengths
#define DEVICE_ADDR_LEN       6
#define MAX_PAYLOAD_SIZE      27

// Channels allowed for use in broadcast.
#define CHANNEL_37            1
#define CHANNEL_38            2
#define CHANNEL_39            4
#define DEVICE_ADDR_LEN       6

// For CC2543 Random Static Address is used by default. This is based on
// production numbers from the infopage and there exist a possibility that
// two CC2543 devices containing the same numbers, but it is unlikely. The
// user is responsible to make sure that a valid address is used. 
//#define DEVICE_ADDRESS_TYPE  DEVICE_ADDRESS_STATIC

//  To use public address the user is resposnible to acquire and load the
//   correct address in flash before uncommenting the line below. 
//#define DEVICE_ADDRESS_TYPE DEVICE_ADDRESS_PUBLIC

#define DEVICE_ADDRESS_TYPE   FW_PARAM_DEVICE_ADDRESS_TYPE
#define FLASH_DEVICE_ADDRESS  FW_PARAM_DEVICE_ADDRESS

/* Data acquisition / sensor reading intervals */
#define INTERVAL_1_mSEC      32         // 1  mSec    equals  32.7530 ticks
#define INTERVAL_2_mSEC      65         // 2  mSec    equals  65.5060 ticks
#define INTERVAL_3_mSEC      98         // 3  mSec    equals  98.2590 ticks
#define INTERVAL_4_mSEC      131        // 4  mSec    equals 131.0120 ticks
#define INTERVAL_5_mSEC      163        // 5  mSec    equals 163.7650 ticks
#define INTERVAL_6_mSEC      196        // 6  mSec    equals 196.5180 ticks
#define INTERVAL_7_mSEC      229        // 7  mSec    equals 229.2710 ticks
#define INTERVAL_8_mSEC      262        // 8  mSec    equals 262.0240 ticks
#define INTERVAL_9_mSEC      294        // 9  mSec    equals 294.7770 ticks
#define INTERVAL_10_mSEC     327        // 10 mSec    equals 327.5300 ticks
#define INTERVAL_20_mSEC     655        // 20 mSec    equals 655.0600 ticks
#define INTERVAL_30_mSEC     982        // 30 mSec    equals 982.5900 ticks
#define INTERVAL_40_mSEC     1310       // 40 mSec    equals 1310.1200 ticks
#define INTERVAL_50_mSEC     1637       // 50 mSec    equals 1637.6500 ticks
#define INTERVAL_60_mSEC     1965       // 60 mSec    equals 1965.1800 ticks
#define INTERVAL_70_mSEC     2292       // 70 mSec    equals 2292.7100 ticks
#define INTERVAL_80_mSEC     2620       // 80 mSec    equals 2620.2400 ticks
#define INTERVAL_90_mSEC     2947       // 90 mSec    equals 2947.7700 ticks

/*  Advertisement intervals ( min = 100ms, 0.625 minimum step, max = 10.24 s).
*   32 kHz RC Oscillator (calibrated = 32.753 kHz)
*   1 tick duration = 3.0531554361432540530638414801698e-5 or 30.53 us
*   100 mSec equals 327.53 ticks
*   0.625 ms equals 20.470625 */
#define INTERVAL_INITIAL       327      // 10   mSec    equals 32.7530 ticks
#define INTERVAL_100_mSEC     3275      // 100  mSec    equals 3275.30 ticks
#define INTERVAL_200_mSEC     6550      // 200  mSec    equals 6550.60 ticks
#define INTERVAL_300_mSEC     9826      // 300  mSec    equals 9825.90 ticks
#define INTERVAL_400_mSEC    13101      // 400  mSec    equals 13101.20 ticks
#define INTERVAL_500_mSEC    16376      // 500  mSec    equals 16376.50 ticks
#define INTERVAL_600_mSEC    19651      // 600  mSec    equals 19651.80 ticks
#define INTERVAL_700_mSEC    22927      // 700  mSec    equals 22927.10 ticks
#define INTERVAL_800_mSEC    26202      // 800  mSec    equals 26202.40 ticks
#define INTERVAL_900_mSEC    29477      // 900  mSec    equals 29477.70 ticks
#define INTERVAL_1_SEC       32753UL      // 1 Sec        equals 32753.00 ticks
#define INTERVAL_2_SEC       65506      // 2 Sec        equals 65506.00 ticks
#define INTERVAL_3_SEC       98259      // 3 Sec        equals 98259.00 ticks
#define INTERVAL_4_SEC      131012      // 4 Sec        equals 131012.00 ticks
#define INTERVAL_5_SEC      163765      // 5 Sec        equals 163765.00 ticks
#define INTERVAL_6_SEC      196518      // 6 Sec        equals 196518.00 ticks
#define INTERVAL_7_SEC      229271      // 7 Sec        equals 229271.00 ticks
#define INTERVAL_8_SEC      262024      // 8 Sec        equals 262024.00 ticks
#define INTERVAL_9_SEC      294777      // 9 Sec        equals 294777.00 ticks
#define INTERVAL_10_SEC     327530      // 10 Sec       equals 327530.00 ticks
#define INTERVAL_10_24_SEC  335391      // 10.24 Sec    equals 335390.72 ticks
/* End of advertise interval range */

#define INTERVAL_1_MIN      1965180             // 1 Minute   equals 1965180 ticks
#define INTERVAL_MAX        0x00FFFFFF          // max(24 bit) = 16777214 = Approx 8.5 minutes

/*  Packet interval between the packets in the same advertisment event
*   (if TX_INTERVAL = 1). For example the time between TX on channel 37,
*   38 and 39 if all broadcast channel as enabled. */
#define PACKET_INTERVAL     INTERVAL_9_mSEC
#endif