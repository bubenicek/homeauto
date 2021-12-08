/******************************************************************************
*   Filename:       miniBLE.c
*   Revised:        $Date: 2014-06-26 12:57:28 +0200 (to, 26 jun 2014) $
*   Revision:       $Revision: 122 $
*
*   Description:    miniBLE API.
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


/******************************************************************************
* INCLUDES
*/
#include "ioCC254x_bitdef.h"
#if(chip==2543)
#include "ioCC2543.h"
#elif (chip==2541)
#include "ioCC2541.h"
#else
#error "Chip not supported!"
#endif
#include "miniBLE.h"
#include "miniBLE_defs.h"
#include "miniBLE_phy.h"
#include "miniBLE_scheduler.h"
#include "prop_regs.h"
#include "hal_int.h"
#include "clock.h"
#include "power_mode.h"
#if(SMARTRF05EB)
#include "hal_button.h"
#include "hal_led.h"
#include "util_lcd.h"
#include "hal_lcd.h"
#endif



/******************************************************************************
* GLOBAL VARIABLES
*/
#if(SMARTRF05EB)
/* Character array used to decode decimal to hexadecimal. */
const char hex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
#endif
uint8 deviceAddress[ DEVICE_ADDR_LEN ];     // index 0..5 is LSO..MSB
uint8 daqSequence = 0;
volatile miniBleTask_t miniBleTask[NUM_TASKS];
volatile uint8 nuOfActiveChannels;



/******************************************************************************
* GLOBAL FUNCTIONS
*/
/******************************************************************************
* @fn          	miniBleInit
*
* @brief       	Initialize miniBLE. The function does not take register changes
*               into account. If RF HW register or other relevant settings are
*               changed which might change the behaviour, then a hard reset is
*               required before running this function to be sure that the correct
*               settings are used.
*
* @param        void
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleInit(void) {

//    uint8 miniBleAddressType = 0;
    // Reset all tasks.
    for(uint8 i=0; i < NUM_TASKS; i++) {
        miniBleTask[i].enable = 0;
        miniBleTask[i].flag = 0;
        miniBleTask[i].interval = INTERVAL_100_mSEC;
        miniBleTask[i].countdown = INTERVAL_100_mSEC;;
    }

    // Initialize the miniBLE PHY layer.
    miniBlePhyInit();

    if (miniBleSetDeviceAddress(DEVICE_ADDRESS_TYPE)) {
        // Error, device address not set.
        return MINIBLE_FAIL;
    }

    if ( ADDR_VALID( deviceAddress ) == FALSE ) {
        // Error, invalid address (0xFFFFFFFFFFFF).
        return MINIBLE_FAIL;
    }

#if(PSEUDO_ADV_DELAY)
    /*  Seed pseudo random number generator with the 2 LSB from the device
    *   address ( number generator used for advDelay ). */
    MINIBLE_SCHEDULER_PSEUDO_INIT(*((uint16*) deviceAddress));
#endif

     CC254x_DISABLE_32KHZ_CALIBRATION();

    return MINIBLE_SUCCESS;
}



/******************************************************************************
* @fn          	miniBleSetDeviceAddress
*
* @brief       	Set device address. Refer to BLUETOOTH SPECIFICATION Version 4.0
*               [Vol 3] section 1.3 - DEVICE ADDRESS and section 10.8 - RANDOM
*               DEVICE ADDRESS and for more details.
*
* @param        Type of address.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleSetDeviceAddress(uint8 type) {
    switch(type) {
    case DEVICE_ADDRESS_NOT_VALID:
        for(uint8 i=0;i<6;i++) {
            deviceAddress[i] = 0xFF;
        }
        break;
    case DEVICE_ADDRESS_STATIC:
        // Acquire static random device address from the info page.
        COPY_DEV_ADDR( deviceAddress, (uint8 *)(INFO_PAGE_CC2543_DIE_ID) );
        // The two most significant bits of the static address shall be equal to ‘1’.
        deviceAddress[5] |= 0xC0;
        break;
    case DEVICE_ADDRESS_PUBLIC:
        /*  a Valid public address must be placed in FLASH prior to running this.
        *   The public address shall be created in accordance with
        *   section 9.2 ("48-bit universal LAN MAC addresses") of the IEEE
        *   802-2001 standard. */
        //if(FLASH_DEVICE_ADDRESS == 0) {
        //    return MINIBLE_FAIL;
        // }
        /*  Suppress this warning as the execution is is not intended to reach
        *   here in this version. */
#pragma diag_suppress=Pe128
        // Copy device address from flash.
        //COPY_DEV_ADDR( deviceAddress, FLASH_DEVICE_ADDRESS);
        break;
    default:
        return MINIBLE_FAIL_INVALID_ARGUMENT;
        break;
    }
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn          	miniBleGetDeviceAddress
*
* @brief       	Return the current device address in use.
*
* @param        Pointer to variable for storing the current device address.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleGetDeviceAddress(uint8* addressVariable) {
    if( addressVariable ) {
        for(uint8 i=0;i<6;i++) {
            // Transfer device address to user variable.
            addressVariable[i] = deviceAddress[i];
        }
    } else {
        // Null-pointer error.
        return MINIBLE_FAIL_INVALID_ARGUMENT;
    }
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn          	miniBleEnableTask
*
* @brief        Enable task.
*
* @param        taskId Id for task.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleEnableTask(uint8 taskId) {
    if(miniBleTask[taskId].enable) {
        // State is already set, no difference between current and argument.
        return MINIBLE_SUCCESS;
    }
    uint8 intState;
    HAL_INT_LOCK(intState); // Enter Critical section.

    // Enable task flag.
    miniBleTask[taskId].enable = 1;

    if(SchedulerEnabledFlag) {
        // Scheduler is running, schedule task.
        miniBleSchedulerScheduleTask(taskId);
    } else {
        // Scheduler is not running, restart scheduler.
        miniBleSchedulerReset();
        return miniBleSchedulerStart();
    }
    HAL_INT_UNLOCK(intState); // Exit Critical section.
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn          	miniBleDisableTask
*
* @brief        Disable task.
*
* @param        taskId Id for task.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleDisableTask(uint8 taskId) {
    if(!miniBleTask[taskId].enable) {
        // State is already set, no difference between current and argument.
        return MINIBLE_SUCCESS;
    }
    uint8 intState;
    HAL_INT_LOCK(intState); // Enter Critical section.

    // Disable task flag.
    miniBleTask[taskId].enable = 0;

    /*  If no tasks are active (i.e. miniBleTask[0-NUM_TASKS].enable == 0), then
    *   the scheduler has already scheduled an upcoming event which will wake
    *   the device and do nothing. */

    HAL_INT_UNLOCK(intState); // Exit Critical section.
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn          	miniBleSetAdvertisingChannels
*
* @brief        Enable or disable advertising channels. The channels are:
*               RF Channel  0,  2402 MHz, Advertising channel 37
*               RF Channel 12,  2426 MHz, Advertising channel 38
*               RF Channel 39,  2480 MHz, Advertising channel 39
*
* @param        channelMap the three least significant bits corresponds to the
*               three broadcast channels 37 (bit 0), 38 (bit 1) and 39 (bit 2).
*               Set to enable and clear to disable channel.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleSetAdvertisingChannels(uint8 channelMap) {
    // transfer channelmap extern variable (ActiveChannels) in phy.
    ActiveChannels = channelMap;

    // Count number of active channels.
    nuOfActiveChannels = 0;
    for(uint8 i=0; i<3; i++) {
        if((channelMap >> i) & 0x01) {
            nuOfActiveChannels++;
        }
    }

    // Set first channel.
    if(ActiveChannels & CHANNEL_37) {
        firstActiveChannel = CHANNEL_37;
        miniBLEPhySetChannel(CHANNEL_37);
    } else if(ActiveChannels & CHANNEL_38) {
        firstActiveChannel = CHANNEL_38;
        miniBLEPhySetChannel(CHANNEL_38);
    } else if(ActiveChannels & CHANNEL_39) {
        firstActiveChannel = CHANNEL_39;
        miniBLEPhySetChannel(CHANNEL_39);
    } else {
        return MINIBLE_FAIL_INVALID_ARGUMENT;
    }
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn           miniBleSetOutputPower
*
* @brief        Set output power. The TXPOWER can be set at any time except
*               during transmission. Critical sections is used to ensure that
*               an update will not occur during TX.
*
* @param        txpower RF output power.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleSetOutputPower(uint8 txpower) {
    // Check if argument is valid, PA bias control must be set to 0x05 for CC2543.
    if((txpower & 0x05) != 0x05) {
        return MINIBLE_FAIL_INVALID_ARGUMENT;
    }
    uint8 intState;
    HAL_INT_LOCK(intState);                 // Enter Critical section.
    if(!miniBleAdvTxDone) {                 // Possible ongoing transmission?
        HAL_INT_UNLOCK(intState);           // Exit Critical section.
        return MINIBLE_FAIL_RADIO_ACTIVE;
    }
    MINIBLE_PHY_SET_OUTPUT_POWER(txpower);  // Set new TXPOWER setting.
    HAL_INT_UNLOCK(intState);               // Exit Critical section.
    return MINIBLE_SUCCESS;
}


/******************************************************************************
* @fn           miniBleSetAdvertisingData
*
* @brief        Load payload into TXFIFO.
*
* @param        payloadBuffer pointer to payload array (buffer).
* @param        length Length of payload.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleSetAdvertisingData(uint8 *payloadBuffer, uint8 length) {
    uint8 retVal, intState;
    HAL_INT_LOCK(intState);                 // Enter Critical section.
    if(!miniBleAdvTxDone) {
        /* Ongoing advertisment. */
        HAL_INT_UNLOCK(intState);           // Exit Critical section.
        retVal = MINIBLE_FAIL_RADIO_ACTIVE;
    } else {
        MINIBLE_PHY_CMD(CMD_TXFIFO_RESET);
        retVal = miniBLEPhyLoadPacket(payloadBuffer, length);
        HAL_INT_UNLOCK(intState);           // Exit Critical section.
    }
    return retVal;
}

/** Send advertising data */
void miniBleSendAdvertinsgData(void)
{
    MINIBLE_PHY_CMD(CMD_TX);
}

/******************************************************************************
* @fn          	miniBleWaitForNextEvent
*
* @brief        Wait for the next event to occur and enter power mode if there
*               is sufficient time until next event.
*
* @param        void
*
* @return       Triggered events since last wakeup.
*/
uint8 miniBleWaitForNextEvent(void) {
    /*  Wait for the next event to occur and enter power mode (sleep) if there
    *   is sufficient time until next event. */
    return miniBleSchedulerEnterSleep();
}


/******************************************************************************
* @fn          	miniBleWait
*
* @brief        Wait. If power saving is enabled the device will enter sleep
*               (PM2) whenever it can, but it will not disturb the scheduler
*               and hence might wake up during this wait time. But the function
*               will not return until the time runs out.
*
* @param        duration Time to wait (or sleep).
*
* @return       void
*/
void miniBleWait(uint32 duration) {
    // Clear flag used to indicate when given duration has expired.
    waitCompleteFlag = 0;

    /*  The part below could be simplified in terms of code readability by using
    *   the function given below, but because the function is likely to be
    *   called at very frequent intervals the macro implementation used instead
    *   will reduce the execution time by reducing the number of function calls
    *   at the expence of a very small increase in code size.

    miniBleEnableTask(TASK_WAIT);
    */
    uint8 intState;
    uint32 sleepTimerValue;
    HAL_INT_LOCK(intState);     // Enter Critical section.
    miniBleTask[TASK_WAIT].enable = 1;
    miniBleTask[TASK_WAIT].countdown = duration;

    /*  Find how much time has passed since previous event and sync the current wait
    *   task relative to the result. Then trigger the scheduler to allow this newly
    *   enabled wait task to be re-scheduled. */
    miniBleTask[TASK_WAIT].countdown +=  miniBleSchedulerGetTimeSincepreviousEvent();

    // Update current trigger value to now.
    MINIBLE_SCHEDULER_READ_TIMER(sleepTimerValue);

    globalWakeupTime = sleepTimerValue;

    SCHEDULER_INT_TRIGGER();    // Trigger scheduler to run (ST_ISR).
    HAL_INT_UNLOCK(intState);   // Exit Critical section.

    /*  Remain in sleep until wait task is complete. If the wait duration is
    *   longer than the DAQ interval, the next DAQ event will be blocked by the
    *   while statment below. */
    while(!waitCompleteFlag) {
        miniBleSchedulerEnterSleep();
    }

    /*  duration (given argument) is reached, return to called location. Due to
    *   the scheduler design which allows main context to be interrupted at any
    *   time (By advertisment or calibration task) there exists a possibility
    *   that the actual duration is more than what was given. */
    return;
}


/******************************************************************************
* @fn           miniBleSetTaskInterval
*
* @brief        Set the interval for the given task.
*
* @param        taskId Id for task.
* @param        interval Interval for task execution.
* @param        now Set true(1) to schedule according to the new interval
*               given immediatewly or set to false(0) to wait until next
*               scheduled evvent is triggered.
*
* @return       Generic miniBLE function return value.
*/
uint8 miniBleSetTaskInterval(uint8 taskId, uint32 interval, uint8 now) {
    if((interval < INTERVAL_1_mSEC) || (interval > INTERVAL_MAX)) {
        return MINIBLE_FAIL_INVALID_ARGUMENT;
    }
    if(taskId == TASK_ADV) {
        if((interval < INTERVAL_100_mSEC) || (interval > INTERVAL_10_24_SEC)) {
            return MINIBLE_FAIL_INVALID_ARGUMENT;
        }
    }
    uint8 intState;
    HAL_INT_LOCK(intState);         // Enter Critical section.
    miniBleTask[taskId].interval = interval;
    if(now) {
        // Schedule the next advertise event according to the new interval now.
        miniBleSchedulerReset();
        HAL_INT_UNLOCK(intState);   // Exit Critical section.
        return miniBleSchedulerStart();
    }
    HAL_INT_UNLOCK(intState);       // Exit Critical section.
    return MINIBLE_SUCCESS;
}


/******************************************************************************
**************  SmartRF05EB peripheral debug functions ** *********************
******************************************************************************/
#if(SMARTRF05EB)
/******************************************************************************
* @fn          miniBlePrintStatistics
*
* @brief       Print statistics to SMartRF05EB LCD screen
*
* @return      void
*/
void miniBlePrintStatistics(void) {
// Update LCD with statistics.
        halLcdDisplayValue(1, "Evnts: ", (int32)adv_events, " ");
        halLcdDisplayValue(2, "Pckts: ", (int32)packets_sent, " ");
        halLcdDisplayValue(3, "PRF_N_TX: ", (uint32)PRF_N_TX, " ");
}


/******************************************************************************
* @fn          miniBlePrintAddress
*
* @brief       Print current device address to SmartRF05EB LCD.
*
* @return      void
*/
void miniBlePrintAddress(uint8* deviceAddress) {
    halLcdClear();
    halLcdWriteLine(1, "miniBLE (CC2543)");
    halLcdWriteLine(2, "Device Address");

    // Update LCD line 3 with device address.
    halLcdWriteChar(3, 0, '0');
    halLcdWriteChar(3, 1, 'x');
    halLcdWriteChar(3, 2, hex[(deviceAddress[0] & 0xF0) >>  4]);
    halLcdWriteChar(3, 3, hex[(deviceAddress[0] & 0x0F)]);
    halLcdWriteChar(3, 4, hex[(deviceAddress[1] & 0xF0) >>  4]);
    halLcdWriteChar(3, 5, hex[(deviceAddress[1] & 0x0F)]);
    halLcdWriteChar(3, 6, hex[(deviceAddress[2] & 0xF0) >>  4]);
    halLcdWriteChar(3, 7, hex[(deviceAddress[2] & 0x0F)]);
    halLcdWriteChar(3, 8, hex[(deviceAddress[3] & 0xF0) >>  4]);
    halLcdWriteChar(3, 9, hex[(deviceAddress[3] & 0x0F)]);
    halLcdWriteChar(3, 10, hex[(deviceAddress[4] & 0xF0) >>  4]);
    halLcdWriteChar(3, 11, hex[(deviceAddress[4] & 0x0F)]);
    halLcdWriteChar(3, 12, hex[(deviceAddress[5] & 0xF0) >>  4]);
    halLcdWriteChar(3, 13, hex[(deviceAddress[5] & 0x0F)]);
    return;
}
#endif