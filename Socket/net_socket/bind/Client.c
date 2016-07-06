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
#include <pthread.h>

void *bind_thread(void *arg);

int main(int argc, char *argv[])
{
	int iRet;
	char exchangeBuffer[1024];
	int sockAlljoyn;
	printf("hello world!\n");
	signal(SIGPIPE, SIG_IGN);
	
	pthread_t pid_bind;
	if(pthread_create(&pid_bind, NULL, bind_thread, NULL) != 0)
	{
		perror("can't create read thread");
		return -1;
	}

  
	while(1)  
	{  
		struct sockaddr_un address;  
		address.sun_family = AF_INET;  
		address.sin_addr.s_addr = htons(INADDR_ANY);
		address.sin_port = htons(6666);
		if( inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
		{
			return 1;
		}

		sockAlljoyn = socket(AF_INET, SOCK_STREAM, 0);//create a tcp socket
		
		iRet = connect(sockAlljoyn, (struct sockaddr *)&address, sizeof(address));  
		if(iRet == -1)  
		{  
			printf("connect failed\n");  
			return 1;  
		}  
		fd_set fSocketSet;
		FD_ZERO(&fSocketSet); 
		FD_SET(sockAlljoyn, &fSocketSet);

		//printf("Please input Send data:\n");
		char *sendstr = "test";
		//scanf("%s",sendstr);
		//printf("read data is %s\n",sendstr);
		if(-1 == write (sockAlljoyn,sendstr,strlen(sendstr)))
		{
			printf("%s\n",strerror(errno));
		}
		//printf("Waiting Data from ALljoyn Router\n");
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
	
	printf("Waiting for thread to finish...\n");
	void *thread_result;
	if(pthread_join(pid_bind, &thread_result) != 0)
	{
		perror("pthread-join failed");
		return -1;
	}

	return 0;
}

void *bind_thread(void *arg)
{
	unsigned short i = 1;
	//t_thread_test * temp2 = Test;
	while(1)
	{
		struct sockaddr_un address;  
		address.sun_family = AF_INET;  
		address.sin_addr.s_addr = htons(INADDR_ANY);
		address.sin_port = htons(6666);
		if( inet_pton(AF_INET, "127.0.0.1", &address.sin_addr) <= 0)
		{
			return 1;
		}

		sockAlljoyn = socket(AF_INET, SOCK_STREAM, 0);//create a tcp socket
		
		iRet = connect(sockAlljoyn, (struct sockaddr *)&address, sizeof(address));  

	}
}
