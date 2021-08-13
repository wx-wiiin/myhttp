#ifndef PTHREAD_LIST_H
#define PTHREAD_LIST_H
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "cnn_event.h"

#include "print_.h"
using namespace std;

#define THREAD_NUM 5
#define METHOM_LEN 10
#define HTTP_TYPE_LEN 10
#define HOST_LEN 25
#define MAX_EVENT 60 
#define CONN_TYPE_LEN 15
#define BODY_LEN (10*1024)
#define DIR_LEN 64
#define STATE_LEN 4
#define SERVER_LEN 20
#define CONTENT_TYPE_LEN 64
void* loop_thread_func(void* args);
int create_thread();
void create_epoll();
void setnumber(struct thread_list_node* head);
struct thread_list_node
{
	thread_list_node():next(NULL){}
	thread_list_node(thread_list_node* node):next(node){}
public:
	int max_event;
	int epoll_root;
	int num;
	int conned;
	int disconned;

	int init_num;
	long long id;

	int session_num;
	struct sockaddr addr;


	struct sockaddr_in* n_addr;
	struct session_list_node* session_head;
	struct thread_list_node*next;
	
};
struct session_list_node{//会话
	session_list_node():next(NULL),request_head(NULL),awnser_head(NULL){}
	struct request_list_node* request_head;
	struct  awnser_list_node* awnser_head;

	int client_fd;
	int request_num;
	struct session_list_node* next;
};
struct request_list_node{
	request_list_node():next(NULL),isUDP(0){}
	char method[METHOM_LEN];
	char http_type[HTTP_TYPE_LEN];
	char host[HTTP_TYPE_LEN];
	char connection[CONN_TYPE_LEN];
	//char body[BODY_LEN];
	char dir_path[DIR_LEN];
	int isUDP;
	int flag;//0可以提供服务，其他拒绝服务
	struct request_list_node* next;
};
struct awnser_list_node{
	awnser_list_node():next(NULL),send_len(0),isUDP(0){
		memset(http_type,'\0',HTTP_TYPE_LEN);
		memset(state_code,'\0',STATE_LEN);
		memset(state_extra,'\0',STATE_LEN);
		memset(connection,'\0',CONN_TYPE_LEN);
		memset(server,'\0',SERVER_LEN);
		memset(content_type,'\0',CONTENT_TYPE_LEN);
		content_length=0;
		memset(body,'\0',BODY_LEN);
		send_len=0;
	}
	char http_type[HTTP_TYPE_LEN];
	char state_code[STATE_LEN];
	char state_extra[STATE_LEN];

	char connection[CONN_TYPE_LEN];
	char server[SERVER_LEN];
	char content_type[CONTENT_TYPE_LEN];
	int content_length;
	union{
		long long int len;//分块字节长4096
		char chunked[8];//chunked
	}Transfer_Encoding; 
	int isChunk;
	
	char body[BODY_LEN];
	int send_len;
	int isUDP;
	struct awnser_list_node*next;
};

extern struct thread_list_node* head;
extern pthread_key_t epoll_root;
extern pthread_mutex_t mutex;
#endif


