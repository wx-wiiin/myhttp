#include "pthread_list.h"
#include "server_socket.cpp"
#include <signal.h>
#define STATE 0

void create_epoll(struct thread_list_node*);
void setnumber(struct thread_list_node* head);
pthread_mutexattr_t mutexattr;
pthread_mutex_t mutex;
pthread_key_t mytmp;
struct thread_list_node* head =
		new struct thread_list_node;

pthread_key_t epoll_root;

void signal_catch(int signum)
{
	#if STATE
	if(signum == SIGPIPE)
	cout<<" signall SIGPIPE recive! "<<endl;
	else
		cout<<"other SIG recived1 "<<endl;
	#endif
}
int create_thread()
{
	pthread_mutexattr_init(&mutexattr);	
	pthread_mutexattr_setpshared(&mutexattr,PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mutex,&mutexattr);
	
	head->init_num=0;
	pthread_key_create(&epoll_root,NULL);

	int ret;
	pthread_t thr;
	pthread_attr_t attr;
	ret = pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
//	if(ret != 0) perror("pthread_attr_setdetached:");
	ret = pthread_attr_init(&attr);
//	if(ret != 0)perror("pthread_attr_init:");
	ret = pthread_create(&thr,&attr,loop_thread_func,(void*)NULL);
//	if(ret!=0) perror("pthread_create:");
	
}
void* loop_thread_func(void* args)
{
	pthread_mutex_lock(&mutex);

	struct sigaction myaction,oldaction;
	myaction.sa_handler = signal_catch;
	int res = sigaction(SIGPIPE,&myaction,&oldaction);
	if(res != 0) perror("sigaction:");
	struct thread_list_node* tmp = new struct thread_list_node;
	tmp->next = head->next;
	head->next = tmp;
	setnumber(head);
	tmp->max_event = MAX_EVENT;
	tmp->conned = 0;
	tmp->disconned=0;
	tmp->session_head = NULL;
	create_epoll(tmp);
	struct epoll_event* evt = (struct epoll_event*)malloc
		(sizeof(struct epoll_event)*tmp->max_event);
	int nready;
	++head->init_num;
	tmp->init_num = head->num;

	pthread_mutex_unlock(&mutex);
	
	while(head->init_num<THREAD_NUM);//初始化完成	
	int* myepoll_fd = (int*)pthread_getspecific(epoll_root);
	struct thread_list_node*h = head->next;
	while(h->next!=NULL)
	{
		if(h->epoll_root == *myepoll_fd) break;
		h = h->next;
	}

	struct cnn_event myevent;
	while(1)
	{
//		cout<<"[ pthread_list.cpp 79 ]:thread id ["<<h->num<<"]: waiting"<<endl;
		
		nready = epoll_wait(tmp->epoll_root,evt,MAX_EVENT,-1);
		for(int i=0;i<nready;++i)
		{
			
			if(evt[i].events & EPOLLIN){
			//	cout<<"EOLLIN coming"<<endl;
				myevent.cnn_read(&evt[i]);
				}
			else if(evt[i].events & EPOLLOUT){
			//	cout<<"EPOLLOUT COMING"<<endl;
				myevent.cnn_write(&evt[i]);
				}
			else if(evt[i].events & EPOLLRDHUP)
				myevent.cnn_close(&evt[i]);
			//并不会触发RD 通过EPOLLIN 与read判断

		}
	}
}


#if 1
void done(struct thread_list_node* h,int& count)
{
	if(h==NULL) return;
	done(h->next,count);
	h->num = count;
	++count;
}
void setnumber(struct thread_list_node* h)
{
	int count = 0;
	done(h,count);	
}

#endif
void create_epoll(struct thread_list_node*tmp)
{
	int epfd = epoll_create(MAX_EVENT);
	if(epfd <0)perror("epoll_create:");
	tmp->epoll_root = epfd;
	pthread_setspecific(epoll_root,(void*)&tmp->epoll_root);
}
