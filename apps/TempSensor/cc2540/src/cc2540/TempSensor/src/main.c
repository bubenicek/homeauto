/**
 * \file main.c      \brief Vitality sensor initialization module
 */

#include "config.h"

#include "miniBLE.h"
#include "miniBLE_defs.h"
#include "clock.h"
#include "hal_mcu.h"
#include "hal_types.h"
#include "hal_int.h"
#include "string.h"
#if(DTM)
#include "miniBLE_dtm.h"
#endif

#include "temp_sensor.h"


void main(void) 
{
    uint8 events;   
   
    // Initialize system clock source to 32 MHz Xtal.
    // Initialize low speed (sleep timer) clock source to 32 kHz RCOSC. */
    halMcuInit();

    // Initialize the miniBLE  device for broadcast operation. This will
    // initialize the radio and setup the device address.
    miniBleInit();

    // Wait to stabilize voltage after inserting battery ?? Needed ??
    halMcuWaitMs(100);

    // Initialize application.
    temp_sensor_init();
    
    // Set output power to 0 dBm (reduce peak power consumption).
    miniBleSetOutputPower(PLUS_4_DBM);

    // Set advertisement and data acquisition interval.
    miniBleSetTaskInterval(TASK_ADV, CFG_ADVERTISING_INTERVAL, SCHEDULE_NOW);    // Start after first measurement is done
    miniBleSetTaskInterval(TASK_DAQ, INTERVAL_INITIAL, SCHEDULE_NOW);            // Start immediatelly

    // Enable all three broadcast channels.
    miniBleSetAdvertisingChannels(CHANNEL_37 | CHANNEL_38 | CHANNEL_39);

    // Start all tasks.
    miniBleEnableTask(TASK_ADV);        // Start advertisment.
    miniBleEnableTask(TASK_DAQ);        // Start data-aquisition.
    
    while(1) 
    {
        /*  miniBleWaitForNextEvent() must be called in an infinite while loop
        *   for correct operation. The return value indicates which tasks that
        *   has triggered in the sscheduler. Any processing done here
        *   (main context) can be interrupted by the schduler at any time.
        *
        *   Only very short critical sections (< 10 us or more) can be
        *   implemented, otherwise there is a risk of disrupting the
        *   advertisement interval timing. */

        // Wait for next event and enter power mode.
        events = miniBleWaitForNextEvent();

        if (events & BV(TASK_DAQ)) 
        {
            // Process vitality sensor task
            temp_sensor_task();
        }

        if (events & BV(TASK_ADV)) 
        {
            /*  Code can be entered here if the user wants to do any processing
            *   in main context when an advertisment event occurs. Note that there
            *   might be some delay from when the event actually starts until
            *   the code execution reaches this point. */
 
        }
    }
}

void halAssertHandler(void)
{
   while(1);
}