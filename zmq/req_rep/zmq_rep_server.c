#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    printf("Zmq Version:[%d-%d][%d]\n", major, minor, patch);
    
    void * context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_REP);
    zmq_bind(responder, "tcp://127.0.0.1:5555");

    while(1)
    {
		printf("wait client request...\n");
        zmq_msg_t request;
		zmq_msg_init(&request);
		int size = zmq_msg_recv(&request, responder, 0);
		printf("recv hello\n");
		zmq_msg_close(&request);
		
		//TODO:
#if 1
		printf("send 'world' to client...\n");
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, 5);
		memcpy(zmq_msg_data(&reply), "world", 5);
		zmq_msg_send(&reply, responder, 0);
		zmq_msg_close(&reply);	
#endif        
    }
	zmq_close(responder);
	zmq_ctx_destroy(context);
	
	return 0;
}

