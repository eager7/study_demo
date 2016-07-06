/*************************************************************************
	> File Name: main.c
	> Author: 
	> Mail: 
	> Created Time: 2015年11月24日 星期二 16时47分05秒
 ************************************************************************/

#include<stdio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include <json/json.h>
#include <libdaemon/daemon.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/un.h>

#define ALLJOYN_SOCKET_PATH "/tmp/alljoynfd"

void main()
{
    printf("socket program\n");

    int sockAlljoyn = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("sockAlljoyn = %d\n", sockAlljoyn);

    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, ALLJOYN_SOCKET_PATH);

    int iRet = connect(sockAlljoyn, (struct sockaddr *)&address, sizeof(address));
    if(iRet < 0)
    {
        printf("connect failed\n");
    }

    int S = socket(AF_INET, SOCK_STREAM, 0);//create a tcp socket
    printf("S = %d\n", S);
    struct sockaddr_in address2;  
    address2.sin_family = AF_INET;  
    address2.sin_addr.s_addr = htons(INADDR_ANY);
    address2.sin_port = htons(7788);
    connect(S, (struct sockaddr *)&address2, sizeof(address2));




    int sockAlljoyn2 = socket(AF_UNIX, SOCK_STREAM, 0);
    printf("sockAlljoyn2 = %d\n", sockAlljoyn2);
    return 0;
}
