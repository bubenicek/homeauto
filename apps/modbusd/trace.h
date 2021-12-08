
#ifndef __TRACE_H
#define __TRACE_H


//
// Trace options
//
#define ENABLE_TRACE_MODBUS            0
#define ENABLE_TRACE_TCP_SOCKET        0
#define ENABLE_TRACE_SERIAL            1



#define TRACE(_format, ...)  printf(_format "\n", ## __VA_ARGS__)
#define TRACE_PRINTFF(_format, ...)  printf(_format, ## __VA_ARGS__)
#define TRACE_PRINTF(_format, ...)  printf(_format, ## __VA_ARGS__)
#define TRACE_ERROR(_format, ...)  fprintf(stderr, "*** ERROR ***: " _format "\n", ## __VA_ARGS__)


#endif // __TRACE_H
