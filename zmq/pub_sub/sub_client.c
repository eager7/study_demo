#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "zhelpers.h"

int main(void)
{
    void * context = zmq_ctx_new();
    void *subscriber = zmq_socket(context, ZMQ_SUB);
    int ret = zmq_connect(subscriber, "tcp://192.168.4.10:7788");
	assert(ret == 0);
    ret = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0);
	assert(ret == 0);

    char pMsg[1024] = {0};
    for(int i = 0; i < 100; i ++){
        zmq_recv (subscriber, pMsg, sizeof(pMsg), 0);
        printf("recv:%s\n", pMsg);
    }

	zmq_close(subscriber);
	zmq_ctx_destroy(context);
	
	return 0;
}

