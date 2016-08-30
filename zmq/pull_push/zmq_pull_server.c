#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    void * context = zmq_ctx_new();
    void *responder = zmq_socket(context, ZMQ_PUB);
    zmq_bind(responder, "tcp://192.168.4.107:5555");
	
	void * context_r = zmq_ctx_new();
	void *receiver = zmq_socket(context_r, ZMQ_PULL);
	zmq_bind(receiver, "tcp://192.168.4.107:5556");

    while(1)
    {
		
		sleep(1);
		printf("send...\n");
		zmq_msg_t reply;
		zmq_msg_init_size(&reply, 5);
		memcpy(zmq_msg_data(&reply), "world", 5);
		zmq_msg_send(&reply, responder, 0);
		zmq_msg_close(&reply);
		
		sleep(1);
		printf("recv...\n");
        zmq_msg_t request;
		zmq_msg_init(&request);
		int size = zmq_msg_recv(&request, receiver, 0);
		char *string = malloc(size +1);
		memcpy(string, zmq_msg_data(&request), size);
		string[size] = 0;
		printf("recv:%s\n", string);
		zmq_msg_close(&request);
    }
	zmq_close(responder);
	zmq_close(receiver);
	zmq_ctx_destroy(context);
	zmq_ctx_destroy(context_r);
	
	return 0;
}

