
#ifndef __EMON_TASK_H
#define __EMON_TASK_H

#ifndef CFG_EMON_TASK_AVG_TIMEOUT
#define CFG_EMON_TASK_AVG_TIMEOUT      500
#endif

#ifndef CFG_EMON_TASK_ZERO_THRESHOLD
#define CFG_EMON_TASK_ZERO_THRESHOLD   0.07
#endif

typedef struct
{
   uint32_t tmo;
   double irms;
   double irms_sum;
   int irms_count;

} emon_task_t;


/** Start emon task */
int emon_task_start(emon_task_t *s);

/** Get current IRMS */
#define emon_task_get_irms(_s) (_s)->irms

#endif   // __EMON_TASK_H