/******************************************************************************
*   Filename:       hal_battery.c
*   Revised:        $Date: 2014-06-17 14:50:05 +0200 (ti, 17 jun 2014) $
*   Revision:       $Revision: 114 $
*
*   Description:    This file contains the Battery measurement implementation.
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
#include "ioCC2543.h"
#include "hal_types.h"


/******************************************************************************
* CONSTANTS
*/
// ADC voltage levels
#define BATT_ADC_LEVEL_3V           444
#define BATT_ADC_LEVEL_2V           296



/******************************************************************************
* PUBLIC FUNCTIONS
*/


/******************************************************************************
* @fn       battMeasure
*
* @brief    Measure the battery level with the ADC and return
*           it as a percentage 0-100%.
*
* @param    void
*
* @return   Battery level
*/
uint16 battMeasure( void ) 
{
    uint16 adc;

    // Configure ADC and perform a read
    ADCCON3 = 0x2F; //Int. ref; 10 bits ENOB; Input = VDD/3
    while( !(ADCCON1 & 0x80)); //wait for end of conversion

    // Read the result (10 bit resolution)
    adc = ADCL >> 6;
    adc |= ADCH << 2;

    return adc;
}