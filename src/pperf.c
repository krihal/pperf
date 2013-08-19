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

int main(int argc, char *argv[])
{
	struct pperf_config cfg = {0};
	char buf[128];

	if (scan_options(argc, argv, &cfg))
		return -1;

	pr_info("Connect to %s\n", print_addrinfo(cfg.ai_peer, buf, sizeof(buf)));

	return 0;
}
