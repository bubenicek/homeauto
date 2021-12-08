/**
 * \file hal_temp.c         \brief External termistor sensors temperature measurement
 *
 * P0_0 ADC5 A5 vstup ADC pro mìøení úbytku napìtí na diodì
 * P0_1 ADC4 A4 vstup ADC pro mìøení úbytku napìtí na diodì
 * P0_2 ADC3 A3 vstup ADC pro mìøení úbytku napìtí na diodì
 * P0_3 ADC2 A2 vstup ADC pro mìøení úbytku napìtí na diodì
 * P0_4 ADC1 A1 vstup ADC pro mìøení úbytku napìtí na diodì
 * P0_5 DIG OUT D1 digitální vstup pro napájení diody pøes R1 bìhem mìøení
 * P0_6 SCL / ADC ref I2C pro pøipojení senzoru vlhkosti SHT20
 * P0_7 SDA / ADC ref I2C pro pøipojení senzoru vlhkosti SHT20
 *
 * P1_0 DIG OUT D2 digitální vstup pro napájení diody pøes R2 bìhem mìøení
 * P1_1 DIG OUT D3 digitální vstup pro napájení diody pøes R3 bìhem mìøení
 * P1_2 DIG OUT D4 digitální vstup pro napájení diody pøes R4 bìhem mìøení
 * P1_3 DIG OUT D5 digitální vstup pro napájení diody pøes R5 bìhem mìøení
 * P1_4 DIG OUT Zapínání referenèního napìtí pro ADC6 (P0_6)
 * P2_0 DIG OUT Zapínání referenèního napìtí pro ADC7 (P0_7)
 * 
 */


//#include "ioCC2543.h"
#include "hal_sensor.h"
#include "hal_defs.h"
#include "hal_assert.h"
#include "hal_mcu.h"
#include "hal_adc.h"
#include "hal_temp.h"
#include "hal_int.h"

#include "params.h"

// Prototypes:
static void temp_pins_reset(void);
static void enable_temp_pin(temp_pin_e pin, temp_pin_type_e pin_type);
static void disable_temp_pin(temp_pin_e pin);


/** PINs configuration */
static const __code temp_pin_cfg_s temp_pins_cfg[] = 
{
   {PORT0, PIN5},           // D1
   {PORT1, PIN0},           // D2
   {PORT1, PIN1},           // D3
   {PORT1, PIN2},           // D4
   {PORT1, PIN3},           // D5
   {PORT0, PIN4, TRUE, 4},  // A1
   {PORT0, PIN3, TRUE, 3},  // A2
   {PORT0, PIN2, TRUE, 2},  // A3
   {PORT0, PIN1, TRUE, 1},  // A4
   {PORT0, PIN0, TRUE, 0}   // A5
};

/** Diodes configuration */
static const __code temp_diode_cfg_t temp_diodes_cfg[] = 
{
//-----------------------------------------   
//      VDD         GND          ADC
//-----------------------------------------   
   {TEMP_PIN_D5, TEMP_PIN_A4, TEMP_PIN_A5},     // D0
   {TEMP_PIN_D4, TEMP_PIN_A3, TEMP_PIN_A4},     // D1  
   {TEMP_PIN_D3, TEMP_PIN_A2, TEMP_PIN_A3},     // D2  
   {TEMP_PIN_D2, TEMP_PIN_A1, TEMP_PIN_A2},     // D3
   {TEMP_PIN_D3, TEMP_PIN_A4, TEMP_PIN_A3},     // D4
   {TEMP_PIN_D2, TEMP_PIN_A3, TEMP_PIN_A2},     // D5 
   {TEMP_PIN_D1, TEMP_PIN_A2, TEMP_PIN_A1},     // D6
   {TEMP_PIN_D2, TEMP_PIN_A5, TEMP_PIN_A2},     // D7
   {TEMP_PIN_D5, TEMP_PIN_A3, TEMP_PIN_A5},     // D8
   {TEMP_PIN_D3, TEMP_PIN_A1, TEMP_PIN_A3},     // D9
   {TEMP_PIN_D5, TEMP_PIN_A2, TEMP_PIN_A5},     // D10
   {TEMP_PIN_D3, TEMP_PIN_A5, TEMP_PIN_A3},     // D11
   {TEMP_PIN_D1, TEMP_PIN_A3, TEMP_PIN_A1},     // D12 
   {TEMP_PIN_D1, TEMP_PIN_A4, TEMP_PIN_A1},     // D14
   {TEMP_PIN_D5, TEMP_PIN_A1, TEMP_PIN_A5},     // D13
   {TEMP_PIN_D4, TEMP_PIN_A2, TEMP_PIN_A4},     // D15
 //  {TEMP_PIN_D4, TEMP_PIN_A5, TEMP_PIN_A4},     // n-u
 //  {TEMP_PIN_D4, TEMP_PIN_A1, TEMP_PIN_A4},     // n-u
 //  {TEMP_PIN_D1, TEMP_PIN_A5, TEMP_PIN_A1},     // n-u
 //  {TEMP_PIN_D2, TEMP_PIN_A4, TEMP_PIN_A2},     // n-u
};
#define NUM_TEMP_SENSORS (sizeof(temp_diodes_cfg) / sizeof(temp_diode_cfg_t))

void HalTempInit(void)
{
    temp_pins_reset();
}

extern uint16 diode_offset[];

void HalTempRead(uint16 *buf, uint8 bufsize)
{
    int ix;

    HAL_ASSERT(bufsize >= NUM_TEMP_SENSORS);
   
    for (ix = 0; ix < NUM_TEMP_SENSORS; ix++)
    {
        // Enable pins
        enable_temp_pin(temp_diodes_cfg[ix].vdd_pin, TEMP_PIN_TYPE_VDD);
        enable_temp_pin(temp_diodes_cfg[ix].gnd_pin, TEMP_PIN_TYPE_GND);
        enable_temp_pin(temp_diodes_cfg[ix].adc_pin, TEMP_PIN_TYPE_ADC);
     
        // Read value from ADC
        HAL_ASSERT(temp_diodes_cfg[ix].adc_pin < NUM_TEMP_PINS);
        HAL_ASSERT(temp_pins_cfg[temp_diodes_cfg[ix].adc_pin].analog == TRUE);
        
        buf[ix] = HalAdcRead(temp_pins_cfg[temp_diodes_cfg[ix].adc_pin].adc_channel, HAL_ADC_RESOLUTION_12, HAL_ADC_REF_125V);

        // Disable pins
        disable_temp_pin(temp_diodes_cfg[ix].vdd_pin);
        disable_temp_pin(temp_diodes_cfg[ix].gnd_pin);
        disable_temp_pin(temp_diodes_cfg[ix].adc_pin);
    }
}

/** Reset all pins to default state (inputs in high impedance) */
static void temp_pins_reset(void)
{
    // Configure analog pins (P0_0 - P0_7)
    APCFG = 0x00;  // Disable ADC analog pins 
    P0SEL = 0x00;  // Set as GPIO
    P0DIR = 0x00;  // Set as inputs
    P0INP = 0xFF;  // Set 3state
    
    // Configure diode power pins (P1_0 - P1_4)
    P1SEL = 0x00;   // Set as GPIO
    P1DIR = 0x00;   // Set as inputs
    P1INP = 0xFF;   // Set 3state
       
    P2SEL = 0x00;   // Set as GPIO
    P2DIR = 0x00;   // Set as inputs
    P2INP = 0xFF;   // Set 3 state
}

/** Enable (configure) temperature pin */
static void enable_temp_pin(temp_pin_e pin, temp_pin_type_e pin_type)
{
    HAL_ASSERT(pin < NUM_TEMP_PINS);
    
    switch(pin_type)
    {
       case TEMP_PIN_TYPE_VDD:
       case TEMP_PIN_TYPE_GND:
       {
            switch(temp_pins_cfg[pin].port)
            {
                case PORT0:
                    P0SEL &= ~(1 << temp_pins_cfg[pin].pin);   // Set as GPIO
                    P0DIR |= (1 << temp_pins_cfg[pin].pin);    // Set as output
                    if (pin_type == TEMP_PIN_TYPE_VDD)
                       P0 |= (1 << temp_pins_cfg[pin].pin);
                    else
                        P0 &= ~(1 << temp_pins_cfg[pin].pin);                    
                break;

               case PORT1:
                    P1SEL &= ~(1 << temp_pins_cfg[pin].pin);   // Set as GPIO
                    P1DIR |= (1 << temp_pins_cfg[pin].pin);    // Set as output
                    if (pin_type == TEMP_PIN_TYPE_VDD)
                       P1 |= (1 << temp_pins_cfg[pin].pin);
                    else
                        P1 &= ~(1 << temp_pins_cfg[pin].pin);                    
                break;

               case PORT2:
                    P2SEL &= ~(1 << temp_pins_cfg[pin].pin);   // Set as GPIO
                    P2DIR |= (1 << temp_pins_cfg[pin].pin);    // Set as output
                    if (pin_type == TEMP_PIN_TYPE_VDD)
                       P2 |= (1 << temp_pins_cfg[pin].pin);
                    else
                        P2 &= ~(1 << temp_pins_cfg[pin].pin);                    
                break;
            }
       }
       break;
       
       case TEMP_PIN_TYPE_ADC:
       {
            // ADC uses only port0
            APCFG |= (1 << temp_pins_cfg[pin].pin);   // Analog enabled
            P0SEL |= (1 << temp_pins_cfg[pin].pin);   // Set as peripherial
            P0DIR &= ~(1 << temp_pins_cfg[pin].pin);  // Set as input
       }
       break;
    }  
}

/** Disable temperature pin (set pin to high impedance) */
static void disable_temp_pin(temp_pin_e pin)
{
    HAL_ASSERT(pin < NUM_TEMP_PINS);
   
    if (temp_pins_cfg[pin].analog)
    {
        APCFG &= ~(1 << temp_pins_cfg[pin].pin);      // Disable ADC analog pin
    }
    
    switch(temp_pins_cfg[pin].port)
    {
        case PORT0:
            P0SEL &= ~(1 << temp_pins_cfg[pin].pin);   // Set as GPIO
            P0DIR &= ~(1 << temp_pins_cfg[pin].pin);   // Set as inputs
            P0INP |= (1 << temp_pins_cfg[pin].pin);    // Set 3 state
        break;

       case PORT1:
            P1SEL &= ~(1 << temp_pins_cfg[pin].pin);   // Set as GPIO
            P1DIR &= ~(1 << temp_pins_cfg[pin].pin);   // Set as inputs
            P1INP |= (1 << temp_pins_cfg[pin].pin);    // Set 3 state
        break;

        case PORT2:
            P2SEL &= ~(1 << temp_pins_cfg[pin].pin);   // Set as GPIO
            P2DIR &= ~(1 << temp_pins_cfg[pin].pin);   // Set as inputs
            P2INP |= (1 << temp_pins_cfg[pin].pin);    // Set 3 state
        break;
    }
}
