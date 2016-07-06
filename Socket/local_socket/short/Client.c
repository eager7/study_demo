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

#define SOCKET_FILE "/tmp/alljoynfd"

int main(int argc, char *argv[])
{
	int iRet;
	char exchangeBuffer[1024];
	int sockAlljoyn;
	printf("hello world!\n");
	signal(SIGPIPE, SIG_IGN);
	
	
  
	while(1)  
	{  
		sockAlljoyn = socket(AF_UNIX, SOCK_STREAM, 0);//create a tcp socket
		
		struct sockaddr_un address;  
		address.sun_family = AF_UNIX;  
		strcpy(address.sun_path, SOCKET_FILE); 
		iRet = connect(sockAlljoyn, (struct sockaddr *)&address, sizeof(address));  
		if(iRet == -1)  
		{  
			printf("connect failed\n");  
			return 1;  
		}  
		fd_set fSocketSet;
		FD_ZERO(&fSocketSet); 
		FD_SET(sockAlljoyn, &fSocketSet);

		printf("Please input Send data:\n");
		char sendstr[512];
		scanf("%s",sendstr);
		printf("read data is %s\n",sendstr);
		if(-1 == write (sockAlljoyn,sendstr,strlen(sendstr)))
		{
			printf("%s\n",strerror(errno));
		}
		printf("Waiting Data from ALljoyn Router\n");
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
					if(read (sockAlljoyn,exchangeBuffer,sizeof(exchangeBuffer)) > 0)
					{
						printf("read data [%s]\n", exchangeBuffer);
					}
				}
			break;
		}	
	}  	
	close(sockAlljoyn);
	
	return 0;
}