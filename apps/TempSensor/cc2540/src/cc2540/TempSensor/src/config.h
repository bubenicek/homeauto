
#ifndef __CONFIG_H
#define __CONFIG_H


//
// Timing configuration
//
#define CFG_ADVERTISING_INTERVAL      INTERVAL_10_SEC
#define CFG_MEASURING_INTERVAL        (INTERVAL_10_SEC * 6)

//
// Data configuration
//
#define MANUFACTURER_ID                 0xCAFE
#define ADV_DEVICENAME_ID_OFFSET        10              // Device ID offset (Temp xxx)
#define ADV_DATA_TEMP_OFFSET            21              // Temp data offset


#endif   // __CONFIG_H