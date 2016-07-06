#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <memory.h>

typedef struct
{
	unsigned short iRet;
	char buffer[1024];
}t_thread_test;

t_thread_test *Test = NULL;

void *write_thread(void *arg);

int main (int argc, char *argv[])
{
	int Index;
	pthread_t pid_r;
	pthread_t pid_w;
	
	Test = (t_thread_test*)malloc(sizeof(t_thread_test));
	if(Test == NULL)
		exit(1);
			
	if(pthread_create(&pid_w, NULL, write_thread, NULL) != 0)
	{
		perror("can't create read thread");
		return -1;
	}

	//t_thread_test * temp1 = Test;
	while(1)
	{
		sleep(1);
		Index = Test->iRet;
		if(Index == 0)
			printf("read data = [%d]\n",Index);
		//sleep(1);
	}
	printf("Waiting for thread to finish...\n");
	void *thread_result;
	if(pthread_join(pid_w, &thread_result) != 0)
	{
		perror("pthread-join failed");
		return -1;
	}
	
	return 0;
}

void *write_thread(void *arg)
{
	unsigned short i = 1;
	//t_thread_test * temp2 = Test;
	while(1)
	{
		i++;
		Test->iRet = i;
		if(i > 0xfff0)//short used 2 bytes
		printf("write data = [%d]\n",Test->iRet);
		//sleep(1);
	}
}

