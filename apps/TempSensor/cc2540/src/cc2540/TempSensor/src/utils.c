
#include "miniBLE.h"
#include "miniBLE_defs.h"
#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_int.h"

// Length of bd addr as a string
#define B_ADDR_STR_LEN                        15

/* Convert Bluetooth address to string */
char *addr2str(uint8 *pAddr)
{
  uint8       i;
  char        hex[] = "0123456789ABCDEF";
  static char str[B_ADDR_STR_LEN];
  char        *pStr = str;

  // Start from end of addr
  pAddr += DEVICE_ADDR_LEN;

  for ( i = DEVICE_ADDR_LEN; i > 0; i-- )
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }

  *pStr = 0;

  return str;
}
