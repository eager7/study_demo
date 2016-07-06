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

//#define SOCKET_FILE "/tmp/alljoynfd"

int main(int argc, char *argv[])
{
	int iRet;
	char exchangeBuffer[1024];
	int sockAlljoyn;
	printf("hello world!\n");
	//signal(SIGPIPE, SIG_IGN);
	struct sockaddr_in address;  
	address.sin_family = AF_INET;  
	address.sin_addr.s_addr = htons(INADDR_ANY);
	address.sin_port = htons(6667);
	
	if (1 == argc)
	{
		printf ("Please input server's address\n");
		return 1;
	}
	
	if( inet_pton(AF_INET, argv[1], &address.sin_addr) <= 0)
	{
		printf("inet_pton error for %s\n",argv[1]);
		return 1;
	}
	
RE_CONNECT:	
	sockAlljoyn = socket(AF_INET, SOCK_STREAM, 0);//create a tcp socket
	
	while(1)
	{
		printf("try connect to server\n");
		if(-1 == connect(sockAlljoyn, (struct sockaddr *)&address, sizeof(address))) 
		{  
			printf("connect failed...\n");  
			sleep(2);  
		}  
		else
		{
			printf("connect success...\n");  
			break;
		}
	}
	
	while(1)  
	{  
		fd_set fSocketSet;
		FD_ZERO(&fSocketSet); 
		FD_SET(sockAlljoyn, &fSocketSet);

		printf("Please input Send data:\n");
		//char sendstr[512];
		//scanf("%s",sendstr);
		char *sendstr = "test buffer";
		printf("send data is %s\n",sendstr);
		if(-1 == send (sockAlljoyn,sendstr,strlen(sendstr), 0))
		{
			perror("send error\n");
		}
		#if 0
		printf("Waiting Data from ALljoyn Router\n");
		struct timeval time_out = {3,0};
		int select_ret = select(sockAlljoyn +1, &fSocketSet, NULL, NULL, NULL);
		switch(select_ret)
		{
			case 0:
				printf("receive alljoyn message time out \n");
			break;
			case -1:
				printf("receive alljoyn message error \n");	
			break;
			default:
				if(FD_ISSET(sockAlljoyn,&fSocketSet))
				{
					if(recv (sockAlljoyn,exchangeBuffer,sizeof(exchangeBuffer), 0) > 0)
					{
						printf("read data [%s]\n", exchangeBuffer);
					}
					else
					{
						perror("disconnect with server\n");
						close(sockAlljoyn);
						sleep(3);
						goto RE_CONNECT;
					}
				}
			break;
		}
		#endif
		
		sleep(3);
	}  	
	close(sockAlljoyn);
	
	return 0;
}