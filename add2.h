#ifndef ADD2_H
#define ADD2_H
#define ADD EPOLL_CTL_ADD
#define MOD EPOLL_CTL_MOD
#define DEL EPOLL_CTL_DEL

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <time.h>
#include "pthread_list.h"
#include "print_.h"

struct event_add
{
	int _add(int client,struct sockaddr* addr);
	int _mod(int client,int op);
};

#endif