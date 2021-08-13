#ifndef SERVER_SOCKET_
#define SERVER_SOCKET_
       #include <sys/types.h>          /* See NOTES */
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <netinet/ip.h> /* superset of previous */
       #include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
extern int server;
void create_bind_listen(const char* ip,uint16_t port);


#endif