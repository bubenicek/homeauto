/******************************************************************************
*   Filename:       hal_sensor.h
*   Revised:        $Date: 2014-06-17 14:50:05 +0200 (ti, 17 jun 2014) $
*   Revision:       $Revision: 114 $
*
*   Description:    Interface to sensor driver shared code.
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
#ifndef HAL_SENSOR_H
#define HAL_SENSOR_H

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
* INCLUDES
*/
#include "hal_types.h"

/******************************************************************************
* CONSTANTS and MACROS
*/

/* Self test assertion; return FALSE (failed) if condition is not met */
#define ST_ASSERT(cond) st( if (!(cond)) return FALSE; )

/* Ative delay: 125 cycles ~1 msec */
#define ST_HAL_DELAY(n) st( { volatile uint32 i; for (i=0; i<(n); i++) { }; } )

/******************************************************************************
 * FUNCTIONS
 */
uint8 HalSensorReadReg(uint8 addr, uint8 *pBuf, uint8 nBytes);
uint8 HalSensorWriteReg(uint8 addr, uint8 *pBuf, uint8 nBytes);


#ifdef __cplusplus
}
#endif

#endif /* HAL_SENSOR_H */
