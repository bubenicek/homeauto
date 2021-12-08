/******************************************************************************
*   Filename:       miniBLE.h
*   Revised:        $Date: 2014-06-26 12:57:28 +0200 (to, 26 jun 2014) $
*   Revision:       $Revision: 122 $
*
*   Description:    Header file for miniBLE API-implementation.
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
#ifndef MINIBLE_H
#define MINIBLE_H


/*******************************************************************************
 * INCLUDES
 */
#include <hal_types.h>
#include "miniBLE_defs.h"

/*******************************************************************************
* MACROS
*/
/* Simple function to check if address is equal to 0xFFFFFFFFFFFF in which case
 * the address is not valid and the device should not be allowed to advertise.*/
#define ADDR_VALID( DeviceAddress )                                            \
    (!(                                                                        \
    ((DeviceAddress)[0] == 0xFF) &&                                            \
    ((DeviceAddress)[1] == 0xFF) &&                                            \
    ((DeviceAddress)[2] == 0xFF) &&                                            \
    ((DeviceAddress)[3] == 0xFF) &&                                            \
    ((DeviceAddress)[4] == 0xFF) &&                                            \
    ((DeviceAddress)[5] == 0xFF)                                               \
    ))
#define COPY_DEV_ADDR( dstPtr, srcPtr )                                        \
    st (                                                                       \
        (dstPtr)[0] = (srcPtr)[0];                                             \
        (dstPtr)[1] = (srcPtr)[1];                                             \
        (dstPtr)[2] = (srcPtr)[2];                                             \
        (dstPtr)[3] = (srcPtr)[3];                                             \
        (dstPtr)[4] = (srcPtr)[4];                                             \
        (dstPtr)[5] = (srcPtr)[5];                                             \
    )



/*******************************************************************************
* DEFINES
*/
#define DATAACQUISITION             0
#define DATAACQUISITION_REPEATED    1

/* Device address types */
#define DEVICE_ADDRESS_NOT_VALID        0
#define DEVICE_ADDRESS_STATIC           1
#define DEVICE_ADDRESS_PUBLIC           2

/* Information Page and Flash Addresses */
#define INFO_PAGE_FLASH_SIZE                    0x7800      // bits 6..4
#define INFO_PAGE_CC2543_DIE_ID                 0x7FF4      // LSB..MSB

/* Flash Addresses, update the address to a location with a valid address preloaded. */
#ifndef FLASH_DEVICE_ADDRESS
#define FLASH_DEVICE_ADDRESS                    0x0000      // LSB..MSB
#endif

/* Output power settings */
#if(chip==2543) || (chip==2541)
#define PLUS_4_DBM          0xE5
#define PLUS_0_DBM          0xA5
#define MINUS_6_DBM         0x65
#define MINUS_12_DBM        0x45
#define MINUS_15_DBM        0x25
#define MINUS_20_DBM        0x05
#endif


/*******************************************************************************
* GLOBAL VARIABLES
*/
typedef struct {
    uint8	enable;
    uint8	flag;
    uint32	countdown;
    uint32	interval;
} miniBleTask_t;

extern volatile miniBleTask_t miniBleTask[NUM_TASKS];
extern volatile uint8 nuOfActiveChannels;
extern uint8 deviceAddress[];



/******************************************************************************
* GLOBAL FUNCTIONS
*/

/******************************************************************************
**********************  miniBLE API *******************************************
******************************************************************************/
uint8 miniBleInit(void);
uint8 miniBleEnableTask(uint8 taskId);
uint8 miniBleDisableTask(uint8 taskId);
uint8 miniBleSetTaskInterval(uint8 taskId, uint32 interval, uint8 now);
uint8 miniBleSetAdvertisingData(uint8 *payloadBuffer, uint8 length);
uint8 miniBleGetAdvertisingData(uint8 *payloadBuffer);
uint8 miniBleSetAdvertisingChannels(uint8 channelMap);
uint8 miniBleGetAdvertisingChannels(uint8 channelMap);
uint8 miniBleSetOutputPower(uint8 txPower);
uint8 miniBleGetOutputPower(void);
uint8 miniBleSetDeviceAddress(uint8 on);
uint8 miniBleGetDeviceAddress(uint8* addressVariable);
void miniBleWait(uint32 time);
uint8 miniBleWaitForNextEvent(void);
void miniBleSendAdvertinsgData(void);



/******************************************************************************
**************  SmartRF05EB peripheral debug functions ************************
******************************************************************************/
#if(SMARTRF05EB)
void halLcdDisplayValue(uint8 line, char XDATA *pLeft, int32 value, char XDATA *pRight);
void miniBlePrintStatistics(void);
void miniBlePrintAddress(uint8* deviceAddress);
#endif
#endif