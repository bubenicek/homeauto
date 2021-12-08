
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "trace.h"

#if !ENABLE_TRACE_SERIAL
#include "trace_undef.h"
#endif

#define CFG_READ_TIMEOUT	250


int serial_open(const char *name, int baudrate)
{
   int fd;
   struct termios options;

   if ((fd = open(name,  O_RDWR | O_SYNC )) < 0)
      return -1;

   // Get the current options for the port...
   tcgetattr(fd, &options);

   // Set the baud rates
   cfsetispeed(&options, baudrate);
   cfsetospeed(&options, baudrate);

   // Enable the receiver and set local mode
   options.c_cflag |= (CLOCAL | CREAD);
   options.c_cflag &= ~PARENB;
   options.c_cflag &= ~CSTOPB;
   options.c_cflag &= ~CSIZE;
   options.c_cflag |= CS8;

   // vypneme terminalove rizeni linky
   options.c_iflag = IGNPAR;

   options.c_oflag = 0;
   options.c_lflag = 0;       //ICANON;
   options.c_cc[VMIN]=1;
   options.c_cc[VTIME]=0;

   // Set the new options for the port...
   tcsetattr(fd, TCSANOW, &options);

   tcflush(fd, TCIOFLUSH);   // flush the read/write buffer 

   return fd;
}

int serial_close(int sd)
{
   return close(sd);
}

int serial_flush(int sd)
{
   return tcflush(sd, TCIOFLUSH);   // flush the read/write buffer 
}


int serial_write(int sd, void *buf, int count)
{
   int ix;
   
   TRACE_PRINTFF("Serial TX: ");
   for (ix = 0; ix < count; ix++)
   {
      TRACE_PRINTF("%2.2X ", ((uint8_t *)buf)[ix]);
   }
   TRACE_PRINTF("\n");
   
   return write(sd, buf, count);
}

int serial_read(int sd, void *buf, int count)
{
   int ix, res, total = 0;
   fd_set read_fds;
   struct timeval tv;

   while(count > 0)
   {
      // wait for data
      FD_ZERO(&read_fds);
      FD_SET(sd, &read_fds);
      tv.tv_sec = 0;
      tv.tv_usec = CFG_READ_TIMEOUT * 1000;

      if ((res = select(sd+1, &read_fds, NULL, NULL,  &tv)) == 0)
      {
         // timeout
         TRACE_ERROR("Read serial timeout,  remain: %d   read: %d", count, total);
         return -2;
      }
      else if (res < 0)
      {
         TRACE_ERROR("select() failed");
         return -1;
      }

      res = read(sd, buf, count);
      if (res < 0)
      {
         TRACE_ERROR("Read failed");
         return -1;
      }

      count -= res;
      total += res;
   }

   TRACE_PRINTFF("Serial RX: ");
   for (ix = 0; ix < total; ix++)
       TRACE_PRINTF("%2.2X ", ((uint8_t *)buf)[ix]);
   TRACE_PRINTF("\n");
 
   return total;
}

