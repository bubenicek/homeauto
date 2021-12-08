/**
 *  \file
 *  \brief Driver for DS18B20
 *
 *  This is header file for DS18B20.
 *
 */

#include <stdio.h>
#include "hal_types.h"
#include "ds18.h"
#include "ow.h"

void ds18_init(void)
{
    ow_Init();  

    ow_Reset();
    ow_WriteByte(0xCC);
    ow_WriteByte(0x4E);
    ow_WriteByte(0x00);
    ow_WriteByte(0x00);
    ow_WriteByte(0x01);   // 10bit
}

void ds18_deinit(void)
{
    ow_DeInit();
}

void ds18_measure(void)
{
  ow_Reset();
  ow_WriteByte(0xCC);
  ow_WriteByte(0x44);
}

uint16_t ds18_get_temp(void)
{
   uint8_t data;
   uint16_t temp;

   ow_Reset();
   ow_WriteByte(0xCC);
   ow_WriteByte(0xBE);

   ow_ReadByte(&data);
   temp = data;
   ow_ReadByte(&data);
   temp |= (data << 8) & 0xFF00;

   return temp;
}

