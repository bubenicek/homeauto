/******************************************************************************
*   Filename:       hal_sensor.c
*   Revised:        $Date: 2014-06-17 14:50:05 +0200 (ti, 17 jun 2014) $
*   Revision:       $Revision: 114 $
*
*   Description:    This file contains code that is common to all sensor drivers.
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

/*****************************************************************************
* INCLUDES
*/
#include "hal_sensor.h"
#include "hal_i2c.h"
#include "hal_humi.h"


/*******************************************************************************
* LOCAL VARIABLES
*/
static uint8 buffer[24];


/*******************************************************************************
* LOCAL FUNCTIONS
*/
/******************************************************************************
* @fn       HalSensorReadReg
*
* @brief    This function implements the I2C protocol to read from a sensor.
*           The sensor must be selected before this routine is called.
*
* @param    addr Register to read
* @param    pBuf Pointer to buffer to place data
* @param    nBytes Numbver of bytes to read
*
* @return   TRUE if the required number of bytes are reveived
*/
bool HalSensorReadReg(uint8 addr, uint8 *pBuf, uint8 nBytes) {
    uint8 i = 0;

    // Send address we're reading from.
    if (HalI2CWrite(1,&addr) == 1) {
        // Now read data.
        i = HalI2CRead(nBytes,pBuf);
    }

    return i == nBytes;
}


/******************************************************************************
* @fn       HalSensorWriteReg
*
* @brief    This function implements the I2C protocol to write to a sensor.
*           The sensor must be selected before this routine is called.
*
* @param    addr Which register to write
* @param    pBuf Pointer to buffer containing data to be written
* @param    nBytes Number of bytes to write
*
* @return   TRUE if successful write
*/
bool HalSensorWriteReg(uint8 addr, uint8 *pBuf, uint8 nBytes) {
    uint8 i;
    uint8 *p = buffer;

    // Copy address and data to local buffer for burst write.
    *p++ = addr;
    for (i = 0; i < nBytes; i++) {
        *p++ = *pBuf++;
    }
    nBytes++;

    // Send address and data.
    i = HalI2CWrite(nBytes, buffer);

    return (i == nBytes);
}