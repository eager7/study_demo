#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    void * context = zmq_ctx_new();
    void *requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, "tcp://127.0.0.1:5555");

    int request_nbr;
	for (request_nbr = 0; request_nbr != 10; request_nbr++)
    {
		printf("send hello to server...\n");
		zmq_msg_t request;
		zmq_msg_init_size(&request, 5);
		memcpy(zmq_msg_data(&request), "hello", 5);
		printf("sending hello:%d\n", request_nbr);
		zmq_msg_send(&request, requester, 0);
		zmq_msg_close(&request);		
		
		//TODO:
		sleep(1);
#if 1		
 		printf("recv world form server...\n");
        zmq_msg_t reply;
		zmq_msg_init(&reply);
		int size = zmq_msg_recv(&reply, requester, 0);
		printf("recv world\n");
		zmq_msg_close(&reply);
#endif        
	}
	zmq_close(requester);
	zmq_ctx_destroy(context);
	
	return 0;
}

