#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netdb.h>
#include <pthread.h>

struct pserver_config {
	struct addrinfo ai_control;
	int control;
};


static const char *global_optargs = "DL:";


static int scan_options(int argc, char *argv[], struct pserver_config *config)
{
	int c, s;
	struct addrinfo *ai_listen = NULL;
	struct addrinfo hints = {0};
	while ((c = getopt(argc, argv, global_optargs)) != -1) {
		switch (c) {
		case '?':
		/*TODO: help string*/
		exit(1);
		case 'D':
		/*TODO: daemonize code*/
		break;
		case 'L':
		hints.ai_family = AF_UNSPEC;
		hints.ai_flags = AI_NUMERICHOST;
		if ((s = getaddrinfo(optarg, NULL, &hints, &ai_listen)) != 0) {
			perror("getaddrinfo");
			printf("%s", gai_strerror(s));
			return -EINVAL;
		}
		memcpy(&config->ai_control, ai_listen, sizeof(struct addrinfo));
		freeaddrinfo(ai_listen);
		break;
		default:
		return -EINVAL;
		}
	}
	return 0;
}

int setup_pserver(struct pserver_config *config)
{
	const char on = 1;
	config->control = socket(config->ai_control.ai_family, SOCK_STREAM, 0);
	setsockopt(config->control, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	bind(config->control, (struct sockaddr*)&config->ai_control,
	     config->ai_control.ai_addrlen);
	/*TODO: error checking..*/

	return 0;
}

void pserver_loop(struct pserver_config *config)
{
	fd_set fds;
	int nfds;
	
	while (1) {
		/*FIXME: select(), spawn pthread slave etc...*/
		FD_ZERO(&fds);
		FD_SET(config->control, &fds);
		if ((nfds = select(1, &fds, NULL, NULL, NULL)) < 0) {
			perror("select");
			exit(-1);
		}
		


	}
}


int main(int argc, char *argv[])
{

	struct pserver_config config = {{0}};
	if (scan_options(argc, argv, &config))
		return -1;

	if (setup_pserver(&config))
		return -1;

	pserver_loop(&config);
	
	return 0;
}
