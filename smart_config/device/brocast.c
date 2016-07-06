#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include <arpa/inet.h>
#include<netdb.h>

#define PORT 7777
#define MAXDATASIZE 256

int main(int argc,char *argv[])
{
	printf("brocast program\n");
	
    struct sockaddr_in brocast_addr;
    brocast_addr.sin_family=AF_INET;
    brocast_addr.sin_port=htons(PORT);
    brocast_addr.sin_addr.s_addr=inet_addr("255.255.255.255");
    bzero(&(brocast_addr.sin_zero),8);
    	
    int socket_fd;
    if((socket_fd=(socket(AF_INET,SOCK_DGRAM,0)))==-1) {
        perror("socket");
        exit(1);
    }
	
	int on = 1;  /*SO_REUSEADDR port can used twice by program */
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); 

    int so_broadcast=1;
    setsockopt(socket_fd,SOL_SOCKET,SO_BROADCAST,&so_broadcast,sizeof(so_broadcast));
	
	
    char buf[256];
    sprintf(buf,"%s", "Hello, Server");
    sendto(socket_fd,buf,strlen(buf),0,(struct sockaddr *)&brocast_addr,sizeof(brocast_addr));
	
	memset(buf, 0, sizeof(buf));
    socklen_t size=sizeof(brocast_addr);
    recvfrom(socket_fd,buf,MAXDATASIZE,0,(struct sockaddr *)&brocast_addr,&size);
    
    printf("recv :%s\n",buf);

    return 0;
}