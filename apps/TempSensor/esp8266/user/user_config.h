
#ifndef __USER_CONFIG_H
#define __USER_CONFIG_H

#define CFG_SLEEP_SECONDS					15

/* NOTICE---this is for 512KB spi flash.
 * you can change to other sector if you use other size spi flash. */
#define ESP_PARAM_START_SEC				0x3D

#define WRITE_URL									"http://api.thingspeak.com/update"
//#define WRITE_KEY									"A7E6HESRP8IDOM09"		// Test1
#define WRITE_KEY									"OG3G82MX6VF15VKW"			//Test2



#endif // __USER_CONFIG_H
