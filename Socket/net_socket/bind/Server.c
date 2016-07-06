#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/un.h>
#include <string.h>

int main(int argc, char *argv[])
{
	printf("hello world!\n");
	
	/* create a socket */  
	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);  

	struct sockaddr_in server_addr;  
	server_addr.sin_family = AF_INET;  
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(6666);

	// 设置套接字选项避免地址未释放造成使用错误  
    int on=1;  
    if((setsockopt(server_sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
    {  
        perror("setsockopt failed");  
        exit(EXIT_FAILURE);  
    }  
	
	/* bind with the local file */  
	if(-1 == bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)))
	{
		perror("bind error!");
		return 1;
	}		

	/* listen */  
	if(-1 == listen(server_sockfd, 5) ) 
	{
		printf("listen error!");
		return 1;
	}		

	char ch[1024] = {0};  
	char SendBuf[1024] = {0};
	int client_sockfd;  
	struct sockaddr_un client_addr;  
	socklen_t len = sizeof(client_addr);  
	while(1)  
	{  
		printf("server waiting:\n");  
		memset(ch,0,sizeof(ch));
		/* accept a connection */  
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &len);  

		/* exchange data */  
		read(client_sockfd, ch, sizeof(ch));  
		printf("get char from client: %s\n", ch);  
		  
		sprintf(SendBuf,"get data [%s]",ch);  
		write(client_sockfd, SendBuf, sizeof(SendBuf));  

		/* close the socket */  
		close(client_sockfd);  
		//sleep(3);
	}  	

	return 0;
}
