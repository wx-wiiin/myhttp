#ifndef CNN_EVENT_H
#define CNN_EVENT_H
#include<iostream>
#include <unistd.h>
 #include <sys/epoll.h>
#include <string.h>
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <unistd.h>
     #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

#include "pthread_list.h"
#include "add2.h"
using namespace std;


struct cnn_event
{
	int cnn_read(struct epoll_event *event);
	int cnn_write(struct epoll_event *event);
	int cnn_close(struct epoll_event *event);

};
#endif