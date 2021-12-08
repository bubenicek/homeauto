/*******************************************************************************
*  Filename:        adc.c
*  Revised:         $Date: 2014-01-10 14:24:45 +0100 (fr, 10 jan 2014) $
*  Revision:        $Revision: 11715 $
*
*  Description:     Contains functions/macros to simplify usage of the ADC.
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
*******************************************************************************/

/*******************************************************************************
* INCLUDES
*/
#include "adc.h"


/*******************************************************************************
* LOCAL FUNCTIONS
*/
/*******************************************************************************
* @fn  adcSampleSingle
*
* @brief
*      This function makes the adc sample the given channel at the given
*      resolution with the given reference.
*
* @param uint8 reference
*          The reference to compare the channel to be sampled.
*        uint8 resolution
*          The resolution to use during the sample (8, 10, 12 or 14 bit)
*        uint8 input
*          The channel to be sampled.
*
* @return int16
*          The conversion result
*
*******************************************************************************/
int16 adcSampleSingle(uint8 reference, uint8 resolution, uint8 channel) {
    int16 value;

    ADC_ENABLE_CHANNEL(channel);

    ADCIF = 0;
    ADC_SINGLE_CONVERSION(reference | resolution | channel);
    while(!ADCIF);

    value  = (ADCH << 8) & 0xff00;
    value |= ADCL;

    ADC_DISABLE_CHANNEL(channel);

    //  The variable 'value' contains 16 bits where
    //     bit 15 is a sign bit
    //     bit [14 .. 0] contain the absolute sample value
    //     Only the r upper bits are significant, where r is the resolution
    //     Resolution:
    //        12   -> [14 .. 3] (bitmask 0x7FF8)
    //        10   -> [14 .. 5] (bitmask 0x7FE0)
    //         9   -> [14 .. 6] (bitmask 0x7FC0)
    //         7   -> [14 .. 8] (bitmask 0x7F00)

    return value;
}