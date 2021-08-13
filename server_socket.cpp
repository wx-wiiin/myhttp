#include "server_socket.h"
int  server;
void create_bind_listen(const char* ip,uint16_t port)
{
	int ret;
	int sock_server = socket(AF_INET,SOCK_STREAM,0);
	if(sock_server<0){perror("socket£º");exit(0);}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	ret = inet_pton(AF_INET,ip,&addr.sin_addr);
	if(ret == -1){perror("inet_pton:");exit(0);}

	ret = bind(sock_server,(struct sockaddr*)&addr,(socklen_t)sizeof(addr));
	if(ret != 0 ){perror("bind:");exit(0);}

	ret  = listen(sock_server,0);
	if(ret != 0 ){perror("listen:");exit(0);}
	
	server = sock_server;

}
