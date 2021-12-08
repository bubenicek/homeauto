/******************************************************************************
*   Filename:       hal_humi.h
*   Revised:        $Date: 2014-06-17 14:50:05 +0200 (ti, 17 jun 2014) $
*   Revision:       $Revision: 114 $
*
*   Description:    Interface to the humidity sensor driver
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

#ifndef HAL_HUMI_H
#define HAL_HUMI_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * INCLUDES
 */
#include "hal_types.h"


/******************************************************************************
 * CONSTANTS
 */
#define HAL_HUM_MEAS_STATE_1         0    // Start temp. meas.
#define HAL_HUM_MEAS_STATE_2         1    // Read temp. meas, start hum.
#define HAL_HUM_MEAS_STATE_3         2    // Read humi. meas.


/******************************************************************************
 * FUNCTIONS
 */
void HalHumiInit(void);
void HalHumiDeinit(void);
bool HalHumiExecMeasurementStep(uint8 state);
bool HalHumiReadMeasurement(uint8 *pBuf);
bool HalHumiTest(void);

/** Run measurement of temp and humi */
void HalHumiRunMeasurement(uint16 *temp, uint16 *humi);

/**
 * Conversion algorithm, humidity
 */
double HalHumiCalcHumRel(uint16 rawH);

/**
 *  Conversion algorithm, temperature
 */
double HalHumiCalcHumTmp(uint16 rawT);


#ifdef __cplusplus
}
#endif

#endif /* HAL_HUMI_H */