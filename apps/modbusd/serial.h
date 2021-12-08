
#ifndef __SERIAL_H
#define __SERIAL_H

#include <termios.h>

int serial_open(const char *name, int baudrate);
int serial_close(int sd);
int serial_flush(int sd);
int serial_write(int sd, void *buf, int count);
int serial_read(int sd, void *buf, int count);

#endif // __SERIAL_H



