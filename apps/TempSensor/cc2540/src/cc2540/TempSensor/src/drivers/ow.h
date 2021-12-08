/**
 *  \file
 *  \brief Driver for 1-wire bus
 *
 *  This is header file for 1-wire bus.
 *
 */

#ifndef __OW_H
#define __OW_H

void ow_Init(void);
void ow_DeInit();
void ow_Reset(void);
void ow_WriteBit(uint8_t bBit);
uint8_t ow_ReadBit(void);

void ow_WriteByte(uint8_t bByte);
void ow_ReadByte(uint8_t *pbByte);

#endif