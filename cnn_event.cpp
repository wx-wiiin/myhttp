#include "cnn_event.h"

int http_reslution(struct thread_list_node*h,struct epoll_event *event,char buf[])
{
	cout<<"\n*********************cnn_read*********************\n"<<buf<<"\n***************************************************\n"<<endl;

	//找出会话
	int client_fd = event->data.fd;
	struct session_list_node* session_pointer;
	if(h->session_head==NULL)
	{
		h->session_head = new struct session_list_node;
		h->session_head->client_fd = client_fd;
		session_pointer = h->session_head;
		session_pointer->request_num=0;	
		++h->session_num;
	}
	else
	{
		session_pointer = h->session_head;
		while(session_pointer!=NULL)
		{
			if(session_pointer->client_fd==client_fd) break;
			session_pointer=session_pointer->next;
		}
		if(session_pointer==NULL)
		{			
			session_pointer = new struct session_list_node;
			session_pointer->next = h->session_head->next;
			h->session_head->next = session_pointer;
			session_pointer->request_num=0;
			session_pointer->client_fd = client_fd;
			++h->session_num;
		}
	}
	//session_pointer 指向clietn所属的会话
	//找出请求队列pointer
	struct request_list_node* request_pointer = new struct request_list_node;




 //request_pointer中解析报文
	//解析报文
	cout<<"*************request_resolution****************"<<endl;
	struct request_list_node* w = request_pointer;
	w->flag=0;

	//method
	sscanf(buf,"%s %*s",w->method);
	if(strcmp(w->method,"GET")==0)
		cout<<"mothed resolution success:"<<w->method<<endl;
	else
		{ w->flag = -1;}

	//dir
	struct stat lbuf;
	sscanf(buf,"%*[^/]%s",w->dir_path);
	if(strcmp(w->dir_path,"/")==0)
		cout<<w->dir_path<<endl;
	else if(lstat(w->dir_path,&lbuf)==0)
		cout<<w->dir_path<<endl;
	else
		{ w->flag = -1;}


	// http 1.1 only
	sscanf(buf,"%*[^H]%s",w->http_type);
	if(strcmp(w->http_type,"HTTP/1.1")==0)
		cout<<w->http_type<<endl;
	else 
		{ w->flag = -1;}


	//host
	memset(w->host,'\0',HTTP_TYPE_LEN);
	sscanf(buf,"%*[^H]%*s%*[^H]%*s%s",w->host);
	if(strlen(w->host)!=0)
		cout<<w->host<<endl;
	
	memset(w->connection,'\0',CONN_TYPE_LEN);
	sscanf(buf,"%*[^C]%*s%s",w->connection);
	if(strlen(w->connection)!=0)
	{
		cout<<w->connection<<endl;
		if(strcmp(w->connection,"UDP???????设置UDP标识"))
			w->isUDP=1;
	}
	
	memset(buf,'\0',BODY_LEN);
//request_pointer中解析报文

		if(session_pointer->request_head==NULL)
	{
		session_pointer->request_head = request_pointer;
	}else
	{
		struct request_list_node* t = session_pointer->request_head;
		while(t->next!=NULL)
			t = t->next;
		t->next = request_pointer;
	}
		++session_pointer->request_num;
	return 0;
}

int cnn_event::cnn_read(struct epoll_event *event)
{
		
	int* epoll_fd = (int*)pthread_getspecific(epoll_root);
		struct thread_list_node*h = head->next;
		while(h->next!=NULL)//前面都没有，事件肯定是最后一个的
	{
		if(h->epoll_root == *epoll_fd) break;
		h = h->next;
	}
	char ip[15];
	int port = htons(h->n_addr->sin_port);

	char buf[BODY_LEN];
	ssize_t len = read(event->data.fd,buf,sizeof(buf));

	if(len==0) {
		
		this->cnn_close(event);
		inet_ntop(AF_INET,&h->n_addr->sin_addr,ip,sizeof(ip));

//		cout<<endl<<"[ pthread_list.cpp 86 ]: "<<" revive ip "
//				<<ip<<"@:"<<port<<" closed"<<endl;
//			cout<<endl;
			Print();
			return -2;
				
		}
	else{
			int ret = http_reslution(h,event,buf);
			//if(ret!=-1)
				reinterpret_cast<struct event_add*>(NULL)->_mod(event->data.fd,EPOLLOUT);
				cout<<"this time file"<<endl;
			//else
			//	cout<<"connot to EPOLLOUT "<<endl;
		}
	Print();
	return 0;
}
int create_body_chuncked(struct awnser_list_node* t,const char* path,int isUDP)
{
		struct stat buf_get_file_len;
		int res = lstat(path,&buf_get_file_len);
		if(res == -1){perror("get_file_len filed!");return -1;}
		int file_len = buf_get_file_len.st_size;//文件字节数
		unsigned int awnser_num = file_len/(BODY_LEN) + 1;
		char buf[BODY_LEN]={'\0'};
		int file_fd = open(path,O_RDONLY);
		if(file_fd<=0) perror("file_fd open:");
		int len=0;
		struct awnser_list_node*nt;
		for(int i=1;i<=awnser_num;++i)
		{
			 nt = new struct awnser_list_node;
				strcpy(nt->http_type,"HTTP/1.1");                    //////////////////////////////////发送的中间部分
				strcpy(nt->server,"wiiin_server");
				strcpy(nt->state_code,"200");
				strcpy(nt->state_extra,"ok");
				strcpy(nt->content_type,t->content_type);
				lseek(file_fd,(i-1)*BODY_LEN,SEEK_SET);
				len = read(file_fd,nt->body,BODY_LEN);
				nt->send_len = len;
				sprintf(nt->Transfer_Encoding.chunked,"%d",len);         /////////////////////////////发送的尾部
				nt->isChunk = 1;
				nt->isUDP = isUDP;
			t->next = nt;
			t=nt;
		}
		 nt = new struct awnser_list_node;
                  // 响应报文添加尾
				strcpy(nt->body,"\r\n");
				nt->isChunk = 2;
				nt->send_len = strlen(nt->body);
				nt->isUDP = isUDP;
			t->next = nt;
			t=nt;

}
int http_write(struct epoll_event *event)//创建awnser ,根据request
{
	int* epoll_fd = (int*)pthread_getspecific(epoll_root);

	struct thread_list_node* h = head->next;
	while(h->next!=NULL)//前面都没有，事件肯定是最后一个的
	{
		if(h->epoll_root == *epoll_fd) break;
		h = h->next;
	}//找到对应的线程信息
	int client_fd = event->data.fd;
	struct session_list_node* session_pointer = h->session_head;
	while(session_pointer!=NULL)
	{
		if(session_pointer->client_fd == client_fd) break;
		session_pointer = session_pointer->next;
	}
	
	//session_pointer 指向requet_head 、 awnser_head 解析拷贝赋值，待发送
#if 0

	char buf_head[1024]={'\0'};
	char buf_body[1024*1024]={'\0'};
	char buf_body2[1024*1024]={'\0'};
	char buf_end[1024]={'\0'};

	sprintf(buf_head,"HTTP/1.1 200 ok\r\n"
	//	"Content-type: image/png\r\n"//;base64
	"Content-type: application/pdf\r\n"//;base64
	"Transfer_Encoding: chunked\r\n\r\n");
	int len_head = strlen(buf_head); //-----------------------------------------//报文头 chunked
	

	int fd = open("/home/http_resource/resume.pdf",O_RDONLY);
	lseek(fd,0,SEEK_SET);
	int len_body = read(fd,buf_body,10*1024);//HTTP/1.1 200 ok\r\n ------------------//报文体
	lseek(fd,len_body,SEEK_SET);
	int len_body2 = read(fd,buf_body2,1024*1024);//HTTP/1.1 200 ok\r\n ------------------//报文体

	int n_fd = open("/home/http_resource/main_1.cpp",O_RDWR|O_CREAT,0755);
	lseek(n_fd,0,SEEK_SET);
	int rlen = write(n_fd,buf_body,len_body);
	lseek(n_fd,rlen,SEEK_SET);
	write(n_fd,buf_body2,len_body2);
	sprintf(buf_end,"\r\n");
	int len_end = strlen(buf_end); //-----------------------------------------//报文头 chunked
	

	cout<<"len_sum "<<len_head+len_body+len_body2+len_end<<endl;
	cout<<"buf_head "<<len_head<<endl;
	cout<<"buf_body "<<len_body<<endl;
	cout<<"buf_body2 "<<len_body2<<endl;
	cout<<"buf_end  "<<len_end<<endl;

	int fd_2 = open("/home/http_resource/main_1_head.cpp",O_RDWR|O_CREAT,0755);
	write(fd_2,buf_head,len_head);

	send(client_fd,buf_head,len_head,0);
	send(client_fd,buf_body,len_body,0);
	send(client_fd,buf_body2,len_body2,0);
	//	send(client_fd,buf_end,len_end,0);

	close(client_fd);
	return 1;

#endif

	struct session_list_node* w = session_pointer;
	//新建发送节点 t 
	struct awnser_list_node* t = new struct awnser_list_node;
	struct awnser_list_node* wt = w->awnser_head;
	if(wt==NULL)
	{
		w->awnser_head = t;
	}else
	{
		while(wt->next!=NULL)
			wt=wt->next;
		wt->next=t;
	}//节点挂在

	
	if(w->request_head->flag!=0 || strcmp(w->request_head->method,"GET")!=0)//只向应 GET方法
	{
	
		strcpy(t->http_type,w->request_head->http_type);                    // 响应报文添加头
	    strcpy(t->content_type,"text/txt;charset=utf-8");
		strcpy(t->state_code,"403");//0 组建错误码  HTTP  403  \r\n  close\r\r server\r\n
		strcpy(t->state_code,"ok");
		strcpy(t->server,"wiiin");
		strcpy(t->Transfer_Encoding.chunked,"chunked");
		t->isChunk = 0;
		t->isUDP = w->request_head->isUDP;
		return -1;

	}
	else
	{
		strcpy(t->http_type,w->request_head->http_type);       //////////////////////////////////////发送头部
		strcpy(t->state_code,"200");
		strcpy(t->server,"wiiin_server");
		strcpy(t->state_extra,"ok");

	
		if(strcmp(w->request_head->dir_path,"/")==0||strcmp(w->request_head->dir_path,"/home/http/")==0)	
			strcpy(t->content_type,"application/pdf");      //默认pdf
		else 
		{	
				char n_buf[20]={'\0'};
				sscanf(w->request_head->dir_path,"%*[^.].%s",n_buf);//获取后缀名
			if(strlen(n_buf)==0||strcmp(n_buf,"cpp")==0||strcmp(n_buf,"txt")==0  )
			{
				strcpy(t->content_type,"text/txt;charset=utf-8");//普通文本
			}
			else
			{
				if(strcmp(n_buf,"html")==0)
					strcpy(t->content_type,"text/html;charset=utf-8");
				else if(strcmp(n_buf,"pdf")==0)
					strcpy(t->content_type,"application/pdf"); 
				else if(strcmp(n_buf,"ico")==0)
					strcpy(t->content_type,"image/ico");  
				else if(strcmp(n_buf,"doc")==0)
				{strcpy(t->content_type,"application/octet-stream");}
				else if(strcmp(n_buf,"png")==0)
					strcpy(t->content_type,"image/png");
			}
				t->isUDP = w->request_head->isUDP;
		}
		
	}
	strcpy(t->state_code,"ok");
	strcpy(t->server,"wiiin");
	strcpy(t->Transfer_Encoding.chunked,"chunked");
	t->isChunk = 0;
	
	
	if(strcmp(w->request_head->dir_path,"/")==0||strcmp(w->request_head->dir_path,"/home/http/")==0)
	{//创建发送简历   中部＋尾部
		int ret = create_body_chuncked(t,"/home/http/resume.pdf",t->isUDP);
		if(ret == -1)
		{
			strcpy(t->http_type,w->request_head->http_type);                    // 响应报文添加头
			strcpy(t->content_type,"text/txt;charset=utf-8");
			strcpy(t->state_code,"403");//0 组建错误码  HTTP  403  \r\n  close\r\r server\r\n
			strcpy(t->state_code,"ok");
			strcpy(t->server,"wiiin");
			strcpy(t->Transfer_Encoding.chunked,"chunked");
			t->isChunk = 0;
			t->isUDP = w->request_head->isUDP;
			return -1;
		}
	}
	else
	{//其他文本
		int ret = create_body_chuncked(t,w->request_head->dir_path,t->isUDP);
		if(ret == -1)
		{
			strcpy(t->http_type,w->request_head->http_type);                    // 响应报文添加头
			strcpy(t->content_type,"text/txt;charset=utf-8");
			strcpy(t->state_code,"403");//0 组建错误码  HTTP  403  \r\n  close\r\r server\r\n
			strcpy(t->state_code,"ok");
			strcpy(t->server,"wiiin");
			strcpy(t->Transfer_Encoding.chunked,"chunked");
			t->isChunk = 0;
			t->isUDP = w->request_head->isUDP;
			return -1;
		}
	}
	--w->request_num;
	w->request_head = w->request_head->next;
		
}
int cnn_write_done(struct session_list_node* session_pointer,int client_fd)
{
#if 0
	char buf_send[BODY_LEN + 512]={'\0'};
	if(session_pointer->awnser_head->isChunk == 1){//中间
		sprintf(buf_send,"%s",
					session_pointer->awnser_head->body);
	}else	
		if(session_pointer->awnser_head->isChunk == 2){//尾
					sprintf(buf_send,"\r\n");
					//session_pointer->awnser_head->body);
					session_pointer->awnser_head->send_len = strlen("\r\n");
		
	}else{	//头报文								      Transfer_Encoding: 
	#if 0
		sprintf(buf_send,"%s %s ok\r\nContent-type: %s\r\nTransfer_Encoding: %s\r\n\r\n",
					session_pointer->awnser_head->http_type,
					session_pointer->awnser_head->state_code,
					session_pointer->awnser_head->content_type,
					session_pointer->awnser_head->Transfer_Encoding.chunked
			);


			sprintf(session_pointer->awnser_head->body,
				"HTTP/1.1 200 ok\r\n"
				"Content-type: application/pdf\r\n"         //;base64
				"Transfer_Encoding: chunked\r\n\r\n");
	#endif
	#if 0
				sprintf(buf_send,"HTTP/1.1 200 ok\r\n"
				"Content-type: application/pdf\r\n"//;base64
				"Transfer_Encoding: chunked\r\n\r\n");
	#endif
		
		session_pointer->awnser_head->send_len = strlen(buf_send);                           //头部长度确定
	}
#endif 
	if(session_pointer->awnser_head->isUDP==1)
	{
		;//UDP 标识没用到
	
	}
	if(session_pointer->awnser_head->isChunk == 0){ ///////////////////////////////////////头
		sprintf(session_pointer->awnser_head->body,"%s %s ok\r\nConnection: keep-alive\r\nContent-type: %s\r\nTransfer_Encoding: %s\r\n\r\n",
					session_pointer->awnser_head->http_type,
					session_pointer->awnser_head->state_code,
					session_pointer->awnser_head->content_type,
					session_pointer->awnser_head->Transfer_Encoding.chunked);
		session_pointer->awnser_head->send_len = strlen(session_pointer->awnser_head->body);
		int ret = send(client_fd,session_pointer->awnser_head->body,session_pointer->awnser_head->send_len,0);
				#if 1
				int ret2=0;
				while(ret != session_pointer->awnser_head->send_len)
				{
					ret2 = send(client_fd,session_pointer->awnser_head->body+ret,session_pointer->awnser_head->send_len,0);
					ret = ret + ret2;
				}
				#endif
	//	int fd_2 = open("/home/http/main_2_head.cpp",O_RDWR|O_CREAT,0755);
	//	write(fd_2,session_pointer->awnser_head->body,session_pointer->awnser_head->send_len);
		}
			else if(session_pointer->awnser_head->isChunk == 1)//zhengwen 
			{
					int ret = send(client_fd,session_pointer->awnser_head->body,session_pointer->awnser_head->send_len,0);
						#if 0
						int ret2=0;
						while(ret != session_pointer->awnser_head->send_len)
						{
							ret2 = send(client_fd,session_pointer->awnser_head->body+ret,session_pointer->awnser_head->send_len,0);
							ret = ret + ret2;
						}
						#endif
	//	int fd = open("/home/http/main_2.cpp",O_CREAT|O_RDWR,0755);
	//		write(fd,session_pointer->awnser_head->body,session_pointer->awnser_head->send_len);
		}
	else
		if(session_pointer->awnser_head->isChunk == 2)
	{
		;
	}
	
}
int cnn_event::cnn_write(struct epoll_event *event)
{	
	pthread_mutex_lock(&mutex);
	cout<<"******************cnn_write:*****************\n"<<endl;
	http_write(event);//request --- .awnser
	int* epoll_fd = (int*)pthread_getspecific(epoll_root);
	struct thread_list_node* h = head->next;
	while(h->next!=NULL)//前面都没有，事件肯定是最后一个的
	{
		if(h->epoll_root == *epoll_fd) break;
		h = h->next;
	}//找到对应的线程信息
	int client_fd = event->data.fd;
	struct session_list_node* session_pointer = h->session_head;

	while(session_pointer!=NULL)//将该会话的所有awnser发送出去
	{
		if(session_pointer->client_fd == client_fd) break;
		session_pointer = session_pointer->next;
	}
	struct awnser_list_node* rm_rf;
	
	while(session_pointer->awnser_head!=NULL)
	{

		cnn_write_done(session_pointer,client_fd);
		rm_rf = session_pointer->awnser_head;
		session_pointer->awnser_head = session_pointer->awnser_head->next;
		delete(rm_rf);
	}
	reinterpret_cast<struct event_add*>(NULL)->_mod(client_fd,EPOLLIN);
	reinterpret_cast<struct cnn_event*>(NULL)->cnn_close(event);
	close(client_fd);
	Print();
	//开启时机 EPOLLIN开启时机
	pthread_mutex_unlock(&mutex);
	return 0;
}
int cnn_event::cnn_close(struct epoll_event *event)
{
	int* epoll_fd = (int*)pthread_getspecific(epoll_root);
	struct thread_list_node*h = head->next;
		while(h->next!=NULL)
	{
		if(h->epoll_root == *epoll_fd) break;
		h = h->next;
	}
	int client_fd = event->data.fd;
	int ret = epoll_ctl(*epoll_fd,EPOLL_CTL_DEL,client_fd,NULL);
	//if(ret == -1)perror("epoll_ctl:");	
	struct session_list_node* t = h->session_head;
	if(t==NULL) return 1;
	while(t->next!=NULL)
	{
		if(t->next->client_fd == client_fd)break;
		t=t->next;
	}
	if(t == h->session_head)
	{
		h->session_head=NULL;
		--h->session_num;
	}else
	{
		t=t->next;
		--h->session_num;
	}
	++h->disconned;
}