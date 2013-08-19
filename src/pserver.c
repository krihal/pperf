#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "netlib.h"
#include "tipc.h"


#define PSERVER_DEFAULT_LISTEN_PORT "4999"
#define PSERVER_DEFAULT_LISTEN_ADDR "::"

struct pserver_config {
	struct addrinfo *ai_listen;
	int sd_listen;
};


/*This struct defines the testcase entry point for a
 * given ether proto/ulp */
struct protocol_handler {
	uint16_t	ether_proto;
	uint16_t	__PAD;
	uint32_t	ul_proto;
	void		(*hdl)(int sd_control, void *param);
};

/*TODO: make a static array of PH's, or a linked list?*/
static const struct protocol_handler ph = {
	.ether_proto	= 0x88CA,
	.ul_proto	= 0,
	.hdl		= tipc_proto_handler
};

static const char *global_optargs = "DL:p:v";

static int verbosity = 0;

#ifndef pr_info
#define pr_info(fmt, ...) {if(verbosity >= 1) \
				printf("info: " fmt, __VA_ARGS__);}
#define pr_err(fmt, ...) { printf("error: " fmt, __VA_ARGS__);}

#endif /* pr_info */

static int scan_options(int argc, char *argv[],
			struct pserver_config *cfg)
{
	int c;
	char *hostname	= PSERVER_DEFAULT_LISTEN_ADDR;
	char *port	= PSERVER_DEFAULT_LISTEN_PORT;

	while ((c = getopt(argc, argv, global_optargs)) != -1) {
		switch (c) {
		case '?':
		/*TODO: help string*/
		exit(1);
		case 'D':
		/*TODO: daemonize code*/
		break;
		case 'v':
		verbosity = 1;
		break;
		case 'L':
			hostname = optarg;
		break;
		case 'p':
			port = optarg;
		break;
		default:
		return -EINVAL;
		}
	}
	get_addrinfo_byname(hostname, port, &cfg->ai_listen);
	return 0;
}

int setup_pserver(struct pserver_config *config)
{
	const int on = 1;
	char buf[128];

	if ((config->sd_listen = socket(config->ai_listen->ai_family,
				 config->ai_listen->ai_socktype,
				 config->ai_listen->ai_protocol)) == -1) {
		perror("socket");
		exit(-1);
	}

	if (setsockopt(config->sd_listen, SOL_SOCKET, SO_REUSEADDR, &on,
		   sizeof(on)) != 0) {
		perror("setsockopt");
		exit(-1);
	}
	if (bind(config->sd_listen, (struct sockaddr*)config->ai_listen->ai_addr,
	     config->ai_listen->ai_addrlen) != 0) {
		perror("bind");
		exit(-1);
	}
	if (listen(config->sd_listen, 10) != 0) {
		perror("listen");
		exit(-1);
	}
	pr_info("Pserver listening on %s\n", 
		print_addrinfo(config->ai_listen, buf, sizeof(buf)));
	return 0;
}

void process_request(int sd_control)
{
	struct pperf_request req = {0};

	if (recv(sd_control, &req, sizeof(req), 0) != sizeof(req))
	{
		perror("Failed to read request from client");
		exit(-1);
	}
	pr_info("pserver received: ether_proto = 0x%x ul_proto = %d\n",
		req.ether_proto, req.ul_proto);

	/*TODO: find a registered test module that wants 
	 * this specific ether proto/ulp. For now, just call
	 * the tipc one*/

	if ((req.ether_proto == ph.ether_proto) &&
	    (req.ul_proto == ph.ul_proto)) {
		ph.hdl(sd_control, (void*)req.param);
	}
	else {
		pr_err("No handler registered for ether proto 0x%x and ulp=%d\n",
			req.ether_proto, req.ul_proto);
		exit(-1);
	}
}

void pserver_loop(struct pserver_config *config)
{
	int sd_control;
	struct sockaddr_storage ss_peer;
	struct sockaddr *sa = (struct sockaddr*)&ss_peer;
	socklen_t ss_len;
	char buf[128];

	while (1) {
		ss_len = sizeof(struct sockaddr_storage);
		memset(&ss_peer, 0, ss_len);
		if ((sd_control = accept(config->sd_listen, sa, &ss_len)) == -1) {
		     perror("accept");
		     exit(-1);
		}
		pr_info("Client %s connected\n", print_sockaddr(sa, ss_len, buf, sizeof(buf)));

		/*Fork out a pserver slave that handles all requests
		  for this control connection. 
		  The parent pserver proc continues accepting*/
		if (fork()) {
			process_request(sd_control);
			/*TODO: need to a well-defined and generic
			  interface for the control connection.*/

			/*AF/proto specific testcase runs here*/

			exit(0);
		}
	}
}


int main(int argc, char *argv[])
{

	struct pserver_config config = {0};

	if (scan_options(argc, argv, &config))
		return -1;

	if (setup_pserver(&config))
		return -1;

	pserver_loop(&config);
	
	return 0;
}
