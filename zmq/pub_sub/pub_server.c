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
    void *publisher = zmq_socket(context, ZMQ_PUB);
    int ret = zmq_bind(publisher, "tcp://127.0.0.1:5556");
    assert(ret == 0);
    
    while(1)
    {
		s_send(publisher, "publisher");
        printf("send pub msg\n");
        sleep(1);
    }
	zmq_close(publisher);
	zmq_ctx_destroy(context);
	
	return 0;
}

