/******************************************************************************
*   Filename:       hal_humi.c
*   Revised:        $Date: 2014-06-17 14:50:05 +0200 (ti, 17 jun 2014) $
*   Revision:       $Revision: 114 $
*
*   Description:    Driver for the Sensirion SHT21 Humidity sensor
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
#include "hal_humi.h"
#include "hal_sensor.h"
#include "hal_i2c.h"
#include "hal_defs.h"
#include "miniBLE.h"


/******************************************************************************
* DEFINES
*/
#define HAL_SHT21_I2C_ADDRESS   0x40    // Sensor I2C address
#define S_REG_LEN               2
#define DATA_LEN                3

// Internal commands
#define SHT21_CMD_TEMP_T_H      0xE3    // command trig. temp meas. hold master
#define SHT21_CMD_HUMI_T_H      0xE5    // command trig. humidity meas. hold master
#define SHT21_CMD_TEMP_T_NH     0xF3    // command trig. temp meas. no hold master
#define SHT21_CMD_HUMI_T_NH     0xF5    // command trig. humidity meas. no hold master
#define SHT21_CMD_WRITE_U_R     0xE6    // command write user register
#define SHT21_CMD_READ_U_R      0xE7    // command read user register
#define SHT21_CMD_SOFT_RST      0xFE    // command soft reset

#define HUMIDITY		        0x00
#define TEMPERATURE	            0x01

#define USR_REG_MASK               0x38  // Mask off reserved bits (3,4,5)
#define USR_REG_DEFAULT            0x02  // Disable OTP reload
#define USR_REG_RES_MASK           0x7E  // Only change bits 0 and 7 (meas. res.)
#define USR_REG_11BITRES           0x81  // 11-bit resolution


/*******************************************************************************
* Local Variables
*/
static uint8 usr;                         // Keeps user register value
static uint8 buf[6];                      // Data buffer
static bool  success;


/*******************************************************************************
* FUNCTIONS
*/
static void HalHumiSelect(void);
static bool HalHumiReadData(uint8 *pBuf,uint8 nBytes);
static bool HalHumiWriteCmd(uint8 cmd);



/******************************************************************************
* @fn       HalHumiInit
*
* @brief    Initialise the humidity sensor driver
*
* @param    void
*
* @return   void
*/
void HalHumiInit(void) 
{
    // Init I2C configuration.
    P0SEL |= 0xC0;              // Set P0_7 and P0_6 as peripheral I/O (I2C).
    P2DIR |= (1 << 0);          // Set P2_0 to high = hum-power ON
   
    HalHumiSelect();
    
    // Set 11 bit resolution
    HalSensorReadReg(SHT21_CMD_READ_U_R,&usr,1);
    usr &= USR_REG_RES_MASK;
    usr |= USR_REG_11BITRES;
    HalSensorWriteReg(SHT21_CMD_WRITE_U_R,&usr,1);
    success = FALSE;
}

void HalHumiDeinit(void)
{
    // DeInit I2C configuration.
    HalI2CDisable();
    P0SEL &= ~0xC0;              // Set P0_7 and P0_6 as input
    P2DIR &= ~(1 << 0);          // Set P2_0 to low = hum-power OFF
}

/******************************************************************************
* @fn       HalHumiInit
*
* @brief    Initialise the humidity sensor driver
*
* @param    void
*
* @return   boolean value indicating success(!0) or failure (0).
*/
bool HalHumiExecMeasurementStep(uint8 state) 
{
    HalHumiSelect();

    switch (state) 
    {
    case 0:
        // Start temperature read
        success = HalHumiWriteCmd(SHT21_CMD_TEMP_T_NH);
        break;
    case 1:
        // Read and store temperature value
        if (success)
        {
            success = HalHumiReadData(buf, DATA_LEN);
            
            // Start for humidity read
            if (success) 
            {
                success = HalHumiWriteCmd(SHT21_CMD_HUMI_T_NH);
            }
        }          
        break;
    case 2:
        // Read and store humidity value
        if (success) 
        {
            success = HalHumiReadData(buf+DATA_LEN, DATA_LEN);
        }
        break;
    }
    return success;
}


/******************************************************************************
* @fn       HalHumiReadMeasurement
*
* @brief    Get humidity sensor data
*
* @param    pBuf Pointer to buffer to store humidity sensor data.
*
* @return   boolean value indicating success(!0) or failure (0).
*/
bool HalHumiReadMeasurement(uint8 *pBuf) 
{
    // Store temperature
    pBuf[0] = buf[1];
    pBuf[1] = buf[0];

    // Store humidity
    pBuf[2] = buf[4];
    pBuf[3] = buf[3];

    return success;
}

/** Run measurement of temp and humi */
void HalHumiRunMeasurement(uint16 *temp, uint16 *humi)
{
    HalHumiInit();

    HalHumiExecMeasurementStep(0);      // Start temp measurement.
    miniBleWait(INTERVAL_1_mSEC*15);    // Wait/Sleep for 15 ms.
    HalHumiExecMeasurementStep(1);      // Read temp data and start humi measurement.
    miniBleWait(INTERVAL_20_mSEC);      // Wait/Sleep for 20 ms.
    HalHumiExecMeasurementStep(2);      // Read humidity data.
    miniBleWait(INTERVAL_20_mSEC);      // Wait/Sleep for 20 ms.
    
    // Fill outgoing data packet
    *temp = buf[0] << 8 | buf[1];
    *humi = buf[3] << 8 | buf[4];

    HalHumiDeinit();
}

/*******************************************************************************
* LOCAL FUNCTIONS
*/

/******************************************************************************
* @fn       HalHumiSelect
*
* @brief    Select the humidity sensor on the I2C-bus
*
* @param    void
*
* @return   void
*/
static void HalHumiSelect(void) 
{
    //Set up I2C that is used to communicate with SHT21
    HalI2CInit(HAL_SHT21_I2C_ADDRESS,i2cClock_267KHZ);
}


/******************************************************************************
* @fn       halHumiWriteCmd
*
* @brief    Write a command to the humidity sensor
*
* @param    cmd Command to write
*
* @return   boolean TRUE if the command has been transmitted successfully
*/
static bool HalHumiWriteCmd(uint8 cmd) 
{
    /* Send command */
    return HalI2CWrite(1,&cmd) == 1;
}


/******************************************************************************
* @fn       HalHumiReadData
*
* @brief    This function implements the I2C protocol to read from the SHT21.
*
* @param    pBuf Pointer to buffer to place data.
*
* @param    nBytes Number of bytes to read.
*
* @return   boolean TRUE if the required number of bytes are received
*/
static bool HalHumiReadData(uint8 *pBuf, uint8 nBytes) 
{
  /* Read data */
  return HalI2CRead(nBytes,pBuf ) == nBytes;
}

/**
 * Conversion algorithm, humidity
 */
double HalHumiCalcHumRel(uint16 rawH)
{
    double v;

    rawH &= ~0x0003; // clear bits [1..0] (status bits)
    //-- calculate relative humidity [%RH] --
    v = -6.0 + 125.0/65536 * (double)rawH; // RH= -6 + 125 * SRH/2^16

    return v;
}

/**
 *  Conversion algorithm, temperature
 */
double HalHumiCalcHumTmp(uint16 rawT)
{
    double v;

    //-- calculate temperature [°C] --
    v = -46.85 + 175.72/65536 *(double)(uint16)rawT;

    return v;
}