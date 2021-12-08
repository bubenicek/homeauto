
#ifndef __HAL_TEMP_H
#define __HAL_TEMP_H

#define NUM_TEMP_PINS       10


/** Temperature pins */
typedef enum
{
    TEMP_PIN_D1 = 0,
    TEMP_PIN_D2,
    TEMP_PIN_D3,
    TEMP_PIN_D4,
    TEMP_PIN_D5,
    TEMP_PIN_A1,
    TEMP_PIN_A2,
    TEMP_PIN_A3,
    TEMP_PIN_A4,
    TEMP_PIN_A5
    
} temp_pin_e;


/** Pins types */
typedef enum
{
   TEMP_PIN_TYPE_VDD,
   TEMP_PIN_TYPE_GND,
   TEMP_PIN_TYPE_ADC,
   
} temp_pin_type_e;


/** Pin configuration */
typedef struct
{
#define PORT0       0
#define PORT1       1
#define PORT2       2

#define PIN0        0
#define PIN1        1
#define PIN2        2
#define PIN3        3
#define PIN4        4
#define PIN5        5
#define PIN6        6
#define PIN7        7

   uint8 port;
   uint8 pin;
   bool analog;
   uint8 adc_channel;
   
} temp_pin_cfg_s;


typedef struct
{
    temp_pin_e vdd_pin;     // VDD pin
    temp_pin_e gnd_pin;     // GND pin
    temp_pin_e adc_pin;     // ADC pin
   
} temp_diode_cfg_t;


void HalTempInit(void);
void HalTempRead(uint16 *buf, uint8 bufsize);

#endif   // __HAL_TEMP_H