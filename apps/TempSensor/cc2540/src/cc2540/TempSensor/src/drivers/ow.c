/**
 *  \file
 *  \brief Driver for 1-wire bus
 *
 *  This is C file for 1-wire bus.
 *
 */

#include "hal_types.h"
#include "hal_sleep.h"
#include "hal_mcu.h"
#include "ow.h"

//
// P1.5 - Data
// P1.1 - Power (VCC 3,3V)
//

#define ow_IdleBus() do { \
    /* Set as input */ \
    P1DIR &= ~(1 << 5); \
}while(0)

#define ow_ClrBus() do { \
    /* Set as low output */ \
    P1DIR |= (1 << 5);  \
    P1 &= ~(1 << 5);    \
} while(0)

#define ow_ReadBus() ((P1 & (1 << 5)) == (1 << 5))

#define util_DelayUs(_usec)         halMcuWaitUs(_usec) 

#define ENTER_CRITICAL_REGION()     
#define LEAVE_CRITICAL_REGION()     


void ow_Init()
{ 
    // Enable Power
    P1DIR |= (1 << 1);
    P1 |= (1 << 1);
    
    // input pin
    ow_IdleBus();
    ow_Reset();
}

void ow_DeInit()
{
    // Set data GPIO as input
    P1DIR &= ~(1 << 5);

    // Disable Power and set as input
    P1 &= ~(1 << 1);
    P1DIR &= ~(1 << 1);
}

void ow_Reset()
{
  util_DelayUs(1);
  ow_IdleBus();

  ow_ClrBus();
  util_DelayUs(490);

  ow_IdleBus();
  util_DelayUs(490);
}

void ow_WriteBit(uint8_t bBit)
{
  util_DelayUs(1);
  ow_IdleBus();

  ENTER_CRITICAL_REGION();

  ow_ClrBus();
  if (bBit)
  {
    util_DelayUs(2);
    ow_IdleBus();
    util_DelayUs(60);
  }
  else
  {
    util_DelayUs(61);
  }

  ow_IdleBus();

  LEAVE_CRITICAL_REGION();
}

uint8_t ow_ReadBit(void)
{
  uint8_t bit;

  util_DelayUs(1);
  ow_IdleBus();

  ENTER_CRITICAL_REGION();

  ow_ClrBus();
  util_DelayUs(2);
  ow_IdleBus();

  util_DelayUs(8);
  bit = ow_ReadBus();
  util_DelayUs(50);
  ow_IdleBus();

  LEAVE_CRITICAL_REGION();

  return bit;
}

void ow_WriteByte(uint8_t bByte)
{
  uint8_t i;

  for (i = 0; i < 8; i++)
  {
      ow_WriteBit(bByte & 0x01);
      bByte = bByte >> 1;
  }
}

void ow_ReadByte(uint8_t *pbByte)
{
  uint8_t i;

  *pbByte = 0;

  for (i = 0; i < 8; i++)
  {
    *pbByte = *pbByte >> 1;
    if (ow_ReadBit())
    {
      *pbByte |= 0x80;
    }
  }
}


