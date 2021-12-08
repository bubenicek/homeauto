
#include "app.h"
#include "emon.h"
#include "emontask.h"

TRACE_TAG(emontask);

static void emon_task(void *arg)
{
   emon_task_t *s = arg;
   double irms;

   VERIFY(emon_calc_irms(HAL_ADC0, 1480, &irms) == 0);
   
   s->irms_sum += irms;
   s->irms_count++;

   if (hal_time_ms() >= s->tmo)
   {
      s->irms = s->irms_sum / s->irms_count;

      if (s->irms <= CFG_EMON_TASK_ZERO_THRESHOLD)
         s->irms = 0;
                  
      s->irms_sum = 0;
      s->irms_count = 0;
      
      s->tmo = hal_time_ms() + CFG_EMON_TASK_AVG_TIMEOUT;
   }

   os_scheduler_push_task(emon_task, 0, s);  
}

int emon_task_start(emon_task_t *s)
{
   memset(s, 0, sizeof(emon_task_t));

   // Initialize ADC
   VERIFY_FATAL(hal_adc_init(HAL_ADC0) == 0);
   
   // Initialize emonitor
   emon_init(29.1);

   // Run task
   os_scheduler_push_task(emon_task, 0, s);  

   return 0;
}
