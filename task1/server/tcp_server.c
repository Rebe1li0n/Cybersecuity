#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define PORT 8888						/*侦听端口地址*/
#define BACKLOG 2						/*侦听队列长度*/

void reverse(char * from, char * to, int len){
	for(int i = 0; i < len; i++) {
		to[i] = from[len - (i + 1)];
	}
	to[len] = 0;
}

int reversal(char * from, char * to, int len){
	char buffer[100];
	printf("Reversal size: %ld\n", len);
	reverse(from, buffer, len);
	strcpy(to, buffer);
	return len;
}

/*服务器对客户端的处理*/
void * process_conn_server(void * tmp)
{
	int s = *((int *)tmp);
	ssize_t size = 0;
	char buffer[200];							/*数据的缓冲区*/
	
	for(;;){									/*循环处理过程*/		
		memset(buffer, 0, 200);
		size = read(s, buffer, 200);			/*从套接字中读取数据放到													缓冲区buffer中*/
		printf("Receive Size: %ld\n", size);
		if(size == 0) {							/*没有数据*/
			return;	
		}
		char b[100];
		reversal(buffer, b, size);
		write(s, b, size);/*发给客户端*/
	}
	return NULL;
}


int main(int argc, char *argv[])
{
	int ss,sc;		/*ss为服务器的socket描述符，sc为客户端的socket描述符*/
	struct sockaddr_in server_addr;	/*服务器地址结构*/
	struct sockaddr_in client_addr;	/*客户端地址结构*/
	int err;							/*返回值*/
	pid_t pid;							/*分叉的进行ID*/

	/*建立一个流式套接字*/
	ss = socket(AF_INET, SOCK_STREAM, 0);
	if(ss < 0) {							/*出错*/
		printf("socket error\n");
		return -1;	
	}
	
	/*设置服务器地址*/
	bzero(&server_addr, sizeof(server_addr));			/*清零*/
	server_addr.sin_family = AF_INET;					/*协议族*/
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	/*本地地址*/
	server_addr.sin_port = htons(PORT);				/*服务器端口*/
	
	/*绑定地址结构到套接字描述符*/
	err = bind(ss, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(err < 0) {/*出错*/
		printf("bind error\n");
		return -1;	
	}
	
	/*设置侦听*/
	err = listen(ss, BACKLOG);
	if(err < 0) {										/*出错*/
		printf("listen error\n");
		return -1;	
	}
	
		/*主循环过程*/
	for(;;)	{
		socklen_t addrlen = sizeof(struct sockaddr);
		
		sc = accept(ss, (struct sockaddr*)&client_addr, &addrlen); 
		/*接收客户端连接*/
		if(sc < 0) {							/*出错*/
			continue;						/*结束本次循环*/
		}	
		
		pid_t pid = -1;
		printf("Receive Accept!\n");
		pthread_create(&pid, NULL, (void *)process_conn_server, &sc);
	}
}



