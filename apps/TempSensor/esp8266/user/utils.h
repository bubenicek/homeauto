
#ifndef __UTILS_H
#define __UTILS_H

/** Get time fron start system in ms */
#define os_time_ms() (system_get_time() / 1000)

/** Delay ms */
#define os_delay_ms(x) os_delay_us(x*1000)

/** Get current formatted time */
const char *time_now_str(void);


#endif // __UTILS_H
