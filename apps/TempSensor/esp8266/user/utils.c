
#include "os_type.h"
#include "osapi.h"
#include "utils.h"

#define ONE_SECOND  1000
#define SECONDS     60
#define ONE_MINUTE  (ONE_SECOND * 60)
#define MINUTES     60
#define ONE_HOUR    (ONE_MINUTE * 60)
#define HOURS       24


const ICACHE_FLASH_ATTR char *time_now_str(void)
{
    int duration;
    int msec, sec, min, hour;
    static char strtm[32];

    duration = os_time_ms();

    msec = (duration % 1000);
    duration /= ONE_SECOND;
    sec = (int) (duration % SECONDS);
    duration /= SECONDS;
    min = (int) (duration % MINUTES);
    duration /= MINUTES;
    hour = (int) (duration % HOURS);

    os_sprintf(strtm, "%02d:%02d:%02d.%03d", hour, min, sec, msec);

    return strtm;
}
