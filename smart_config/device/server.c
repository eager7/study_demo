#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>
#include <string.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <netdb.h>

int main(int argc, char *argv[])
{
	printf("smart config program\n");
	
	struct sockaddr_in sin;
	bzero(&sin,sizeof(sin));  
    sin.sin_family=AF_INET;  
    sin.sin_addr.s_addr=htonl(INADDR_ANY);  
    sin.sin_port=htons(7777);  
    socklen_t sin_len=sizeof(sin);  
	
	int iSocketFd = socket(AF_INET,SOCK_DGRAM,0);
	bind(iSocketFd,(struct sockaddr *)&sin,sizeof(sin)); 
	
	int on = 1;  /*SO_REUSEADDR port can used twice by program */
    setsockopt(iSocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); 

	
	char message[256] = {0};
	while(1)  
    {  
		memset(message, 0, sizeof(message));
		printf("Wait Message From Client...\n");
        recvfrom(iSocketFd,message,sizeof(message),0,(struct sockaddr *)&sin,&sin_len);  
        printf("Recv Message:%s\n",message);  
		sendto(iSocketFd, "I am Server", strlen("I am Server"), 0,(struct sockaddr *)&sin,sin_len);
    }  
  
    close(iSocketFd);  
	
	return 0;
}