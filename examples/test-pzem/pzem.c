
#include <stdint.h>
#include "pzem.h"


const uint8_t PZEM_SETADDRESS[] = {0xB4, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x1F};
const uint8_t PZEM_GETVOLTAGE[] = {0xB0, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x1B};
const uint8_t PZEM_GETCURRENT[] = {0xB1, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x1C};
const uint8_t PZEM_GETPOWER[] = {0xB2, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x1D};
const uint8_t PZEM_GETENERGY[] = {0xB3, 0xC0, 0xA8, 0x01, 0x02, 0x00, 0x1E};


/** Init PZEM */
int pzem_init(void)
{
   return 0;
}

/** Read PZEM info */
int pzem_read(pzem_info_t *info)
{
   return 0;
}

