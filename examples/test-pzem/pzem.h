
#ifndef __PZEM_H
#define __PZEM_H

typedef struct
{
   
} pzem_info_t;


/** Init PZEM */
int pzem_init(void);

/** Read PZEM info */
int pzem_read(pzem_info_t *info);


#endif // __PZEM_H