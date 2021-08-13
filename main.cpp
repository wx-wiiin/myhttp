#include <iostream>
#include <pthread.h>
#include "pthread_list.h"
#include "server_socket.h"
using namespace std;
#define IP  "10.206.0.4"
#define PORT 7777

void sigfunc(int sig)
{
	if(sig==SIGALRM)
	{
		int fd = open("./errno.txt",O_CREAT|O_RDWR,0755);
	//	write(fd,"breath!",strlen("breath!"));
		alarm(20);
	}
}

int main()
{

	for(int i=0;i<THREAD_NUM;++i)
			create_thread();
	
	//cout<<"----------------------------------------------------------------"<<endl;
	create_bind_listen(IP,PORT);
	int client_fd;
	struct sockaddr_in* addr = new struct sockaddr_in;
	socklen_t len = sizeof(addr);
	
	signal(SIGALRM,sigfunc);
	int fd_client_1 = open("/home/all_run/ip.cnn_7777.txt",O_CREAT|O_APPEND|O_RDWR,0755);
//	write(fd_client_1,"-------system restart!---------\n",strlen("-------system restart!---------\n"));
	
	while(1)
	{
		alarm(20);
		client_fd = accept(server,(struct sockaddr*)addr,&len);
		reinterpret_cast<struct event_add*>(NULL)->_add(client_fd,(struct sockaddr*)addr);
		//cout<<" cnn "<<endl;
	}

	
	

}
