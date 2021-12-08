
#ifndef __TCP_SOCKET_H
#define __TCP_SOCKET_H

#include <netdb.h>

int tcp_socket_create(int port);
int tcp_socket_connect(const char *host, int port);
int tcp_socket_close(int sd);
int tcp_socket_accept(int sd, struct sockaddr_in *remote_addr);
int tcp_socket_send(int sd, const void *buf, int count);
int tcp_socket_recv(int sd, void *buf, int bufsz);
int tcp_socket_readto(int sd, char *buf, int bufsize, char c);


#endif // __TCP_SOCKET_H