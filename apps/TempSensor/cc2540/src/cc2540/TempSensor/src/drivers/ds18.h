/**
 *  \file
 *  \brief Driver for DS18B20
 *
 *  This is header file for DS18B20.
 *
 */

#ifndef __DS18_H_
#define __DS18_H_

void ds18_init(void);
void ds18_deinit(void);
void ds18_measure(void);
uint16_t ds18_get_temp(void);


#endif
