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

#define SOCKET_FILE "/tmp/alljoynfd"
int main(int argc, char *argv[])
{
	printf("hello world!\n");
	
	/* delete the socket file */  
	unlink(SOCKET_FILE);  

	/* create a socket */  
	int server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);  

	struct sockaddr_un server_addr;  
	server_addr.sun_family = AF_UNIX;  
	strcpy(server_addr.sun_path, SOCKET_FILE);  

	/* bind with the local file */  
	bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));  

	/* listen */  
	listen(server_sockfd, 5);  

	char ch[1024] = {0};  
	char SendBuf[1024] = {0};
	int client_sockfd;  
	struct sockaddr_un client_addr;  
	socklen_t len = sizeof(client_addr);
	
	/* accept a connection */  
	client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &len);  
	
	while(1)  
	{  
		printf("server waiting:\n");  
		memset(ch,0,sizeof(ch));

		/* exchange data */  
		read(client_sockfd, ch, sizeof(ch));  
		printf("get char from client: %s\n", ch);  
		  
		sprintf(SendBuf,"get data [%s]",ch);  
		write(client_sockfd, SendBuf, sizeof(SendBuf));  

		/* close the socket */  
		//sleep(3);
	}  	
	close(client_sockfd);  

	return 0;
}