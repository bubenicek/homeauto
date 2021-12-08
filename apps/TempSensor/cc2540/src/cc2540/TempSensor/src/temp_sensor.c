/**
 * \file temp_sensor.c      \brief Temperature sensor control module
 *
 */

#include "config.h"

#include "miniBLE.h"
#include "miniBLE_defs.h"
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_int.h"

#include "drivers/ds18.h"
#include "temp_sensor.h"
#include "utils.h"


// Prototypes:
static uint16 battMeasure(void);

// Locals:
static uint8 initialized;
static temp_sensor_state_e state;
static uint8_t seqn;

// GAP - Advertising data (max size = 31 bytes)
static uint8 advData[27] =
{
  // Flags; this sets the device to use limited discoverable
  // mode (advertises for 30 seconds at a time) instead of general
  // discoverable mode (advertises indefinitely)
  0x02,   // length of this data
  GAP_ADTYPE_FLAGS,
  GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,
   
  // complete name
  10,   // length of this data
  GAP_ADTYPE_LOCAL_NAME_COMPLETE,
  'T',   // 'T'
  'e',   // 'e'
  'm',   // 'm'
  'p',   // 'p'
  ' ',   // ' '
  '0',   // 'x'
  '0',   // 'x'
  '0',   // 'x'
  '0',   // 'x'

  // three-byte broadcast of the data "1 2 3"
  0x03,                                  // length of this data including the data type byte
  GAP_ADTYPE_MANUFACTURER_SPECIFIC,      // manufacturer specific advertisement data type
  (MANUFACTURER_ID >> 8),
  (MANUFACTURER_ID & 0xFF),
  
  // Temperature data
  0x07,
  GAP_ADTYPE_SERVICE_DATA,
  0,    // Seq 
  0,    // Temp MSB byte
  0,    // Temp LSB byte
  0,    // Batlevel MSB
  0,    // Batlevel LSB
};


/** Task initialization */
void temp_sensor_init(void) 
{
    int idx = ADV_DEVICENAME_ID_OFFSET;
    uint8 addr[DEVICE_ADDR_LEN];
    char *sadr;
    
    // Get device address 
    miniBleGetDeviceAddress(addr);
    // Convert to string
    sadr = addr2str(addr);
    // Update device name 
    advData[idx++] = sadr[8];
    advData[idx++] = sadr[9];
    advData[idx++] = sadr[10];
    advData[idx++] = sadr[11];
   
    // Load empty payload for initial broadcast.
    miniBleSetAdvertisingData(advData, sizeof(advData));

    initialized = FALSE;
    seqn = 0;
    state = TEMP_SENSOR_MEASURE;
}

void temp_sensor_task(void)
{
    switch(state)
    {
        case TEMP_SENSOR_MEASURE:
        {
            // Start temp measure
            ds18_init();
            ds18_measure();
            
            // Reschedule task 
            miniBleSetTaskInterval(TASK_DAQ, INTERVAL_200_mSEC, SCHEDULE_NOW);            
            state = TEMP_SENSOR_READ_SENSOR;
        }
        break;
       
        case TEMP_SENSOR_READ_SENSOR:
        {
            uint16 temp, vbat;
            
            // Get temperature and deinitialize sensor (power off)
            temp = ds18_get_temp();
            ds18_deinit();              
          
            // Get battery level
            vbat = battMeasure();
            
            // Update advertisng data
            advData[ADV_DATA_TEMP_OFFSET] = ++seqn;
            advData[ADV_DATA_TEMP_OFFSET+1] = (temp >> 8) & 0xFF;  // MSB byte
            advData[ADV_DATA_TEMP_OFFSET+2] = temp & 0xFF;         
            advData[ADV_DATA_TEMP_OFFSET+3] = (vbat >> 8) & 0xFF;  // MSB byte
            advData[ADV_DATA_TEMP_OFFSET+4] = vbat & 0xFF;         

            // Update advertising data
            miniBleSetAdvertisingData((uint8 *)&advData, sizeof(advData));
            
            if (!initialized)
            {
                initialized = TRUE;
                
                // Send advertising data first time immediatelly
                miniBleSendAdvertinsgData();
            }

            // Reschedule task 
            miniBleSetTaskInterval(TASK_DAQ, CFG_MEASURING_INTERVAL, SCHEDULE_NOW);
            
            state = TEMP_SENSOR_MEASURE;
        }
       break;
    }
}

static uint16 battMeasure(void) 
{
    uint16 adc;

    // Configure ADC and perform a read
    ADCCON3 = 0x2F; //Int. ref; 10 bits ENOB; Input = VDD/3
    while( !(ADCCON1 & 0x80)); //wait for end of conversion

    // Read the result (10 bit resolution)
    adc = ADCL >> 6;
    adc |= ADCH << 2;

    ADCCON3 = 0x00;
    
    return adc;
}
