/******************************************************************************
*   Filename:       hal_i2c.c
*   Revised:        $Date: 2014-06-17 14:50:05 +0200 (ti, 17 jun 2014) $
*   Revision:       $Revision: 114 $
*
*   Description:    This module defines the HAL I2C API for the CC2541ST. It
*                   implements the I2C master.
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

/*******************************************************************************
* INCLUDES
*/
#include "hal_i2c.h"
#include "ioCC2543.h"
#include "hal_types.h"
#include "hal_defs.h"
#include "ioCC254x_bitdef.h"


/*******************************************************************************
* DEFINES
*/
#define I2C_ENS1            BV(6)
#define I2C_STA             BV(5)
#define I2C_STO             BV(4)
#define I2C_SI              BV(3)
#define I2C_AA              BV(2)
#define I2C_MST_RD_BIT      BV(0)  // Master RD/WRn bit to be OR'ed with Slave address.
#define I2C_CLOCK_MASK      0x83
#define I2C_PXIFG           P2IFG
#define I2C_IF              P2IF
#define I2C_IE              BV(1)


/*******************************************************************************
* TYPEDEFS
*/
typedef enum
{
  // HAL_I2C_MASTER mode statuses.
  mstStarted   = 0x08,
  mstRepStart  = 0x10,
  mstAddrAckW  = 0x18,
  mstAddrNackW = 0x20,
  mstDataAckW  = 0x28,
  mstDataNackW = 0x30,
  mstLostArb   = 0x38,
  mstAddrAckR  = 0x40,
  mstAddrNackR = 0x48,
  mstDataAckR  = 0x50,
  mstDataNackR = 0x58,
} i2cStatus_t;


/*******************************************************************************
* MACROS
*/
#define I2C_CLOCK_RATE(x)     st( I2CCON  &=    ~I2C_CLOCK_MASK;    \
                                  I2CCON  |=     x;                 )
#define I2C_SET_NACK()        st( I2CCON &= ~I2C_AA; )
#define I2C_SET_ACK()         st( I2CCON |=  I2C_AA; )

// Enable I2C bus
#define I2C_ENABLE()          st( I2CCON |= (I2C_ENS1); )
#define I2C_DISABLE()         st( I2CCON &= ~(I2C_ENS1); )

// Must clear SI before setting STA and then STA must be manually cleared.
#define I2C_STRT() st (             \
  I2CCON &= ~I2C_SI;                \
  I2CCON |= I2C_STA;                \
  while ((I2CCON & I2C_SI) == 0);   \
  I2CCON &= ~I2C_STA; \
)

// Must set STO before clearing SI.
#define I2C_STOP() st (             \
  I2CCON |= I2C_STO;                \
  I2CCON &= ~I2C_SI;                \
  while ((I2CCON & I2C_STO) != 0);  \
)

// Stop clock-stretching and then read when it arrives.
#define I2C_READ(_X_) st (          \
  I2CCON &= ~I2C_SI;                \
  while ((I2CCON & I2C_SI) == 0);   \
  (_X_) = I2CDAT;                  \
)

// First write new data and then stop clock-stretching.
#define I2C_WRITE(_X_) st (         \
  I2CDAT = (_X_);                  \
  I2CCON &= ~I2C_SI;                \
  while ((I2CCON & I2C_SI) == 0);   \
)


/*******************************************************************************
* Local Variables
*/
static uint8 i2cAddr;  // Target Slave address pre-shifted up by one leaving RD/WRn LSB as zero.


/*******************************************************************************
* FUNCTIONS
*/
/******************************************************************************
* @fn       i2cMstStrt
*
* @brief    Attempt to send an I2C bus START and Slave Address as an I2C
*           bus Master.
*
* @param    RD_WRn The LSB of the Slave Address as Read/~Write.
*
* @return   The I2C status of the START request or of the Slave Address Ack.
*/
static uint8 i2cMstStrt(uint8 RD_WRn) {
  I2C_STRT();

  if (I2CSTA == mstStarted) {
      // A start condition has been transmitted.
      I2C_WRITE(i2cAddr | RD_WRn);
  }

  return I2CSTA;
}


/**************************************************************************************************
 * @fn          HalI2CInit
 *
 * @brief       Initialize the I2C bus as a Master.
 *
 * input parameters
 *
 * @param       address - I2C slave address.
 * @param       clockRate - I2C clock rate.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void HalI2CInit(uint8 address, i2cClock_t clockRate)
{
  i2cAddr = address << 1;

  I2CADR = 0; // no multi master support at this time
  I2C_CLOCK_RATE(clockRate);
  I2C_ENABLE();
}

/**************************************************************************************************
 * @fn          HalI2CRead
 *
 * @brief       Read from the I2C bus as a Master.
 *
 * input parameters
 *
 * @param       len - Number of bytes to read.
 * @param       pBuf - Pointer to the data buffer to put read bytes.
 *
 * output parameters
 *
 * None.
 *
 * @return      The number of bytes successfully read.
 */
uint8 HalI2CRead(uint8 len, uint8 *pBuf)
{
  uint8 cnt = 0;

  if (i2cMstStrt(I2C_MST_RD_BIT) != mstAddrAckR)
  {
    len = 0;
  }

  // All bytes are ACK'd except for the last one which is NACK'd. If only
  // 1 byte is being read, a single NACK will be sent. Thus, we only want
  // to enable ACK if more than 1 byte is going to be read.
  if (len > 1)
  {
    I2C_SET_ACK();
  }

  while (len > 0)
  {
    // slave devices require NACK to be sent after reading last byte
    if (len == 1)
    {
      I2C_SET_NACK();
    }

    // read a byte from the I2C interface
    I2C_READ(*pBuf++);
    cnt++;
    len--;

    if (I2CSTA != mstDataAckR)
    {
      if (I2CSTA != mstDataNackR)
      {
        // something went wrong, so don't count last byte
        cnt--;
      }
      break;
    }
  }
  I2C_STOP();

  return cnt;
}

/**************************************************************************************************
 * @fn          HalI2CWrite
 *
 * @brief       Write to the I2C bus as a Master.
 *
 * input parameters
 *
 * @param       len - Number of bytes to write.
 * @param       pBuf - Pointer to the data buffer to write.
 *
 * output parameters
 *
 * None.
 *
 * @return      The number of bytes successfully written.
 */
uint8 HalI2CWrite(uint8 len, uint8 *pBuf)
{
  if (i2cMstStrt(0) != mstAddrAckW)
  {
    len = 0;
  }

  for (uint8 cnt = 0; cnt < len; cnt++)
  {
    I2C_WRITE(*pBuf++);

    if (I2CSTA != mstDataAckW)
    {
      if (I2CSTA == mstDataNackW)
      {
        len = cnt + 1;
      }
      else
      {
        len = cnt;
      }
      break;
    }
  }

  I2C_STOP();

  return len;
}

/**************************************************************************************************
 * @fn          HalI2CDisable
 *
 * @brief       Places the I2C bus in inactive mode
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
void HalI2CDisable(void) {
  I2C_DISABLE();
}