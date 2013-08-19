#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "netlib.h"

#define PSERVER_DEFAULT_LISTEN_PORT "4999"
#define PSERVER_DEFAULT_LISTEN_ADDR "::"

struct pperf_config {
	struct addrinfo *ai_peer;
	int sd_control;
};

static const char *global_optargs = "H:p:v";

static int verbosity = 0;

#define pr_info(fmt, ...) {if(verbosity >= 1) \
				printf("info: " fmt, __VA_ARGS__);}

static int scan_options(int argc, char *argv[], struct pperf_config *cfg)
{
	int c;
	char *hostname	= PSERVER_DEFAULT_LISTEN_ADDR;
	char *port	= PSERVER_DEFAULT_LISTEN_PORT;

	while ((c = getopt(argc, argv, global_optargs)) != -1) {
		switch (c) {
		case '?':
		/*TODO: help string*/
		exit(1);
		case 'v':
		verbosity = 1;
		break;
		case 'H':
			hostname = optarg;
		break;
		case 'p':
			port = optarg;
		break;
		default:
		return -EINVAL;
		}
	}
	get_addrinfo_byname(hostname, port, &cfg->ai_peer);
	return 0;
}

void connect_to_pserver(struct pperf_config *cfg)
{
	if ((cfg->sd_control = socket(cfg->ai_peer->ai_family,
				      cfg->ai_peer->ai_socktype,
				      cfg->ai_peer->ai_protocol)) == -1) {
		perror("socket");
		exit(-1);
	}
	if (connect(cfg->sd_control, cfg->ai_peer->ai_addr,
		    cfg->ai_peer->ai_addrlen) != 0) {
		perror("Could not connect to server");
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
	struct pperf_config cfg = {0};
	char buf[128];

	if (scan_options(argc, argv, &cfg))
		return -1;

	pr_info("Connect to %s\n", print_addrinfo(cfg.ai_peer, buf, sizeof(buf)));
	connect_to_pserver(&cfg);
	/*Debug code, to be removed*/
	struct pperf_request req = {0};

	req.ether_proto = 0x88CA;
	req.ul_proto = 0;
	sprintf(req.test_name, "BANANARAMA!");
	printf("send crap\n");
	send(cfg.sd_control, &req, sizeof(req), 0);

	return 0;
}
