

#ifndef __DEBUG_H
#define __DEBUG_H

#include "osapi.h"
#include "utils.h"

#ifdef DEBUG
#define TRACE_PRINTF	os_printf
#else
#define TRACE_PRINTF(...)
#endif

#define TRACE(_fmt, ...) TRACE_PRINTF("%s  %-10.10s: " _fmt "\n", time_now_str(), TRACE_TAG, ##__VA_ARGS__)
#define TRACE_ERROR(_fmt, ...) TRACE_PRINTF("%s  %-10.10s: **ERROR** %s:%d " _fmt "\n", time_now_str(), TRACE_TAG, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#define FATAL_ERROR(_fmt, ...) do { \
	TRACE_PRINTF("%s  %-10.10s: **FATAL_ERROR** %s:%d " _fmt "\n", time_now_str(), TRACE_TAG, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
	while(1); \
} while(0)


#endif // __DEBUG_H
