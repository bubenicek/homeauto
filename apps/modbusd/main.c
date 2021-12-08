/*
 * TODO:
 * - dodelat timeouty pro cteni z odpovedi po serialu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include "trace.h"
#include "tcp_socket.h"
#include "serial.h"
#include "modbus.h"

#define MAX_SERVERS_COUNT          8

#define msleep(ms) usleep(ms * 1000)

typedef struct
{
   uint8_t addr;
   uint16_t coils_state;
   
} modbus_server_t;


// Options:
static modbus_server_t servers[MAX_SERVERS_COUNT];
static int servers_cnt = 0;
static const char *devname = NULL;


static void usage(void)
{
   printf("Usage modbusbridge [-options]\n");
   printf("options:\n");
   printf("   -d <serial device name>        Serial device name\n");
   printf("   -a <modbus address>            Address of china bug relays board for fix protocol\n");
   printf("   -wr <addr> <regaddr> <regdata> Write single register\n");
}
            

static modbus_server_t *find_modbus_server(int addr)
{
   int ix;
   modbus_server_t *server = NULL;
   
   for (ix = 0; ix < servers_cnt; ix++)
   {
      if (servers[ix].addr == addr)
      {
         server = &servers[ix];
         break;
      }
   }
   
   return server;
}

int main(int argc, char *argv[])
{
   int res, ix, rsplen, sout;
   int listen_socket, socket;
   struct sockaddr_in remote_addr;
   modbus_server_t *server;
   uint8_t buf[255];
   
   if (argc < 2)
   {
      usage();
      return 1;
   }

   for (ix = 1; ix < argc; ix++)
   {
      if (!strcmp(argv[ix], "-d"))
      {
         devname = argv[++ix];
      }
      else if (!strcmp(argv[ix], "-a"))
      {
         if (servers_cnt == MAX_SERVERS_COUNT)
         {
            TRACE_ERROR("modbus servers maxnum exceeded");
            return 1;
         }
         servers[servers_cnt++].addr = atoi(argv[++ix]);
      }
      else if (!strcmp(argv[ix], "-wr"))
      {
         int addr, regaddr, value;
         
         addr = atoi(argv[++ix]);
         regaddr = atoi(argv[++ix]);
         value = atoi(argv[++ix]);
         
         // Write single register
         if ((sout = serial_open(devname, B9600)) < 0)
         {
            TRACE_ERROR("open serial %s failed", devname);
            return 1;
         }
         
         if (modbus_rtu_write_sigle_register(sout, addr, regaddr, value) < 0)
         {
            TRACE_ERROR("Write register 0x%X = 0x%X to device addr: 0x%X failed", regaddr, value, addr);
            return 1;
         }
         
         TRACE("Write register success");
         
         serial_close(sout);

         return 0;
      }
   }
   
   if (devname == NULL)
   {
      TRACE_ERROR("Not specified serial out device");
      return 1;
   }
   

   if ((sout = serial_open(devname, B9600)) < 0)
   {
      TRACE_ERROR("open serial %s failed", devname);
      return 1;
   }
   TRACE("Open serial port %s", devname);

/*   
   uint16_t state;
   while(1)
   {
      modbus_rtu_read_inputs(sout, 3, 0, 8, &state);
      msleep(500);
   }
*/
   while(1)
   {
      modbus_rtu_write_coil(sout, 3, 0, 0xFF);
      msleep(1000);
      modbus_rtu_write_coil(sout, 3, 0, 0x00);
      msleep(1000);
   }

    
   for (ix = 0; ix < servers_cnt; ix++)
   {
      // Read init coil status
      while(modbus_rtu_read_coils_state_fix(sout, servers[ix].addr, 0, 8, &servers[ix].coils_state) != 0)
      {
         usleep(100 * 1000);
      }
      
      TRACE("Modbus fix servers addr: 0x%X  coils_state: 0x%X", servers[ix].addr, servers[ix].coils_state);
   }
   
   if ((listen_socket = tcp_socket_create(MODBUS_TCP_PORT)) < 0)
   {
      TRACE_ERROR("Create socket");
      return 1;
   }
   
   TRACE("Listening for TCP data ...");
   
   while(1)
   {
      if ((socket = tcp_socket_accept(listen_socket, &remote_addr)) < 0)
      {
         TRACE_ERROR("Accept connection");
         continue;
      }
      
      TRACE("\nNew connection accepted");
      
      while(1)
      {
         if ((res = tcp_socket_recv(socket, buf, sizeof(buf))) == 0)
         {
            TRACE("Connection closed");
            break;
         }
         else if (res < 0)
         {
            TRACE_ERROR("Recv failed");
            break;
         }
         
         // Set defaul response
         rsplen = res;
         
         // Find modbus server for fix
         server = find_modbus_server(buf[MODBUS_TCP_ADDR_IDX]);
         
         switch(buf[MODBUS_TCP_FUNC_IDX])
         {
            case MODBUS_FUNC_READ_COILS:
            {
               uint16_t state = 0;
               uint16_t start_coil;
               uint16_t count;
               
               start_coil = (buf[MODBUS_TCP_DATA_IDX] << 8) | buf[MODBUS_TCP_DATA_IDX+1];
               count = (buf[MODBUS_TCP_DATA_IDX+2] << 8) | buf[MODBUS_TCP_DATA_IDX+3];
               
               if (server != NULL)
               {
                  state = server->coils_state;
               }
               else
               {
                  if (modbus_rtu_read_coils_state(sout, buf[MODBUS_TCP_ADDR_IDX], start_coil, count, &state) < 0)
                  {
                     TRACE_ERROR("modbus_rtu_read_coils_state failed");
                     buf[MODBUS_TCP_FUNC_IDX] |= 0x80;
                  }
               }
                              
               buf[MODBUS_TCP_DATA_IDX] = count / 8;
               if (count / 8 == 1)
               {
                  buf[MODBUS_TCP_DATA_IDX+1] = state;
               }
               else
               {
                  buf[MODBUS_TCP_DATA_IDX+1] = state >> 8;
                  buf[MODBUS_TCP_DATA_IDX+2] = state & 0xFF;
               }
               
               rsplen = MODBUS_TCP_HEADER_SIZE + 2 + (count / 8);
            }
            break;
            
            case MODBUS_FUNC_WRITE_COIL:
            {
               uint16_t coil, state;
               
               coil = (buf[MODBUS_TCP_DATA_IDX] << 8) | buf[MODBUS_TCP_DATA_IDX+1];
               state = (buf[MODBUS_TCP_DATA_IDX+2] << 8) | buf[MODBUS_TCP_DATA_IDX+3];

               if (server != NULL)
               {
                  // FIX. china relay board !!!
                  if (state == 0xFF00)
                  {
                     state = 0x100;
                     server->coils_state |= (1 << coil);
                  }
                  else
                  {
                     server->coils_state &= ~(1 << coil);
                  }
                  
                  // FIX. china relay board !!!, have to begin from 1
                  coil++;
               }

               if (modbus_rtu_write_coil(sout, buf[MODBUS_TCP_ADDR_IDX], coil, state) < 0)
               {
                  TRACE_ERROR("modbus_rtu_write_coil failed");
                  buf[MODBUS_TCP_FUNC_IDX] |= 0x80;
               }
            }
            break;
            
            case MODBUS_READ_DISCRETE_INPUTS:
            {
               uint16_t start_input, count;
               uint16_t state = 0;
               
               start_input = (buf[MODBUS_TCP_DATA_IDX] << 8) | buf[MODBUS_TCP_DATA_IDX+1];
               count = (buf[MODBUS_TCP_DATA_IDX+2] << 8) | buf[MODBUS_TCP_DATA_IDX+3];

               if (server != NULL)
               {
                  // FIX china relay board !!
                  start_input = 0;
                  count = 0;
               }
              
               if (modbus_rtu_read_inputs(sout, buf[MODBUS_TCP_ADDR_IDX], start_input, count, &state) < 0)
               {
                  TRACE_ERROR("modbus_rtu_read_inputs failed");
                  buf[MODBUS_TCP_FUNC_IDX] |= 0x80;
               }

               if (server != NULL)
               {
                  // FIX china relay board !!
                  start_input = 0;
                  count = 8;
               }

               buf[MODBUS_TCP_DATA_IDX] = count / 8;
               if (count / 8 == 1)
               {
                  buf[MODBUS_TCP_DATA_IDX+1] = state;
               }
               else
               {
                  buf[MODBUS_TCP_DATA_IDX+1] = state >> 8;
                  buf[MODBUS_TCP_DATA_IDX+2] = state & 0xFF;
               }
               
               rsplen = MODBUS_TCP_HEADER_SIZE + 2 + (count / 8);
            }
            break;
               
            default:
               TRACE_ERROR("Not supported modbus func: 0x%X", buf[MODBUS_TCP_FUNC_IDX]);
         }
         
         // Send response
         if (tcp_socket_send(socket, buf, rsplen) != rsplen)
         {
            TRACE_ERROR("Send response failed");
         }
      }
      
      tcp_socket_close(socket);
   }
   
   tcp_socket_close(listen_socket);
   
   return 0;
}
