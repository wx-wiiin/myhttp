#include "add2.h"


int event_add::_add(int client,struct sockaddr* addr)
{
	thread_list_node* h = head->next;
	thread_list_node* tmp=h;
	while(h!=NULL)
	{
		if((tmp->conned - tmp->disconned)>(h->conned - h->disconned))
			tmp = h;
		h=h->next;
	}
	h=tmp;
	if(h->conned-h->disconned>h->max_event)
		return -1;
	++h->conned;
	//h->addr = (struct sockaddr)*addr;
	struct sockaddr_in* t = (struct sockaddr_in*)addr;
	h->n_addr = new struct sockaddr_in;
	h->n_addr->sin_family = t->sin_family;
	h->n_addr->sin_port = t->sin_port;
	h->n_addr->sin_addr = t->sin_addr;
	struct epoll_event evt;
	evt.events = EPOLLIN|EPOLLRDHUP|EPOLLET;
	evt.data.fd = client;
	epoll_ctl(h->epoll_root,EPOLL_CTL_ADD,client,&evt);

	struct sockaddr_in* n_h = (struct sockaddr_in*)addr;
	char ip[15]={'\0'};
	int port = ntohs(n_h->sin_port);
			//struct sockaddr_in* myaddr = &((struct sockaddr_in)h->addr);
			inet_ntop(AF_INET,&n_h->sin_addr,ip,sizeof(ip));
		
//			cout<<endl<<"[ pthread_list.cpp 86 ]: "<<" revive ip "
//				<<ip<<"@:"<<port<<" request"<<endl;
			int fd_client = open("/home/all_run/ip.cnn.txt",O_CREAT|O_APPEND|O_RDWR,0755);
			ip[strlen(ip)]='\n';
			time_t timep = time(NULL);
            struct tm* tt = localtime(&timep);
            char *buf_t = asctime(tt);
	//		write(fd_client,buf_t,strlen(buf_t));
	//		write(fd_client,ip,strlen(ip));
//			cout<<endl;
	Print();
return 0;

}
int event_add::_mod(int client,int op)
{
//监听读事件
//return -1;
	if(op == EPOLLOUT){
	int* epoll_fd = (int*)pthread_getspecific(epoll_root);
	struct epoll_event evt;
	evt.data.fd = client;
	evt.events = EPOLLOUT|EPOLLET;//|EPOLLIN;要想设置EPOLLIN也为关注事件，EPOLLIN应该选为
	int ret = epoll_ctl(*epoll_fd,EPOLL_CTL_MOD,client,&evt);
//	if(ret == -1) perror("epoll_ctl_MOD_OUT:");
	}else
	if(op == EPOLLIN)
	{
			int* epoll_fd = (int*)pthread_getspecific(epoll_root);
	struct epoll_event evt;
	evt.data.fd = client;
	evt.events = EPOLLIN|EPOLLET;//|EPOLLIN;要想设置EPOLLIN也为关注事件，EPOLLIN应该选为
	int ret = epoll_ctl(*epoll_fd,EPOLL_CTL_MOD,client,&evt);
//	if(ret == -1) perror("epoll_ctl_MOD_IN:");


	}
}	