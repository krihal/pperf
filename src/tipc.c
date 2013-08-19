#include <linux/tipc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct tipc_request {
	uint32_t	socktype;
	uint8_t 	dest_droppable;
	uint8_t		__PAD1;
	uint16_t	__PAD2;
};

struct tipc_response {
	uint32_t	node;
	uint32_t	ref;
};

void tipc_proto_handler(int control_sd, void *param)
{
	struct tipc_request *treq = (struct tipc_request*)param;

	printf("TIPC protocol handler socktype = %d\n",treq->socktype);
}
