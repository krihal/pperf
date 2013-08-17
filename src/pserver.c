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
#include <netdb.h>
#include <pthread.h>

#define PSERVER_DEFAULT_LISTEN_PORT "4999"
#define PSERVER_DEFAULT_LISTEN_ADDR "::"

struct pserver_config {
	struct addrinfo ai_listen;
	int sd_listen;
	int sd_control;
};

struct pserver_thread {
	pthread_t id;
	int sd_control;
	LIST_ENTRY(pserver_thread) next;
};

LIST_HEAD(threads_head, pserver_thread) pserver_threads;

static const char *global_optargs = "DL:p:v";

static int verbosity = 0;

#define pr_info(fmt, ...) {if(verbosity >= 1) \
				printf("info: " fmt, __VA_ARGS__);}

static void create_listen_sa(char *hostname, char *port,
			     struct pserver_config *cfg)
{
	int s;
	struct addrinfo hints = {0};
	struct addrinfo *ai_listen;

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_NUMERICHOST;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if ((s = getaddrinfo(hostname, port, &hints, &ai_listen)) != 0) {
		perror("getaddrinfo");
		printf("%s", gai_strerror(s));
		exit(-1);
	}
	memcpy(&cfg->ai_listen, ai_listen, sizeof(struct addrinfo));
	freeaddrinfo(ai_listen);

}

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
	pr_info("pserver listening on %s port %s\n", hostname, port);
	create_listen_sa(hostname, port, cfg);
	return 0;
}

int setup_pserver(struct pserver_config *config)
{
	const char on = 1;

	config->sd_listen = socket(config->ai_listen.ai_family,
				 config->ai_listen.ai_socktype,
				 config->ai_listen.ai_protocol);
	setsockopt(config->sd_listen, SOL_SOCKET, SO_REUSEADDR, &on,
		   sizeof(on));
	bind(config->sd_listen, (struct sockaddr*)&config->ai_listen,
	     config->ai_listen.ai_addrlen);
	/*TODO: error checking..*/
	listen(config->sd_listen, 10);
	return 0;
}

void *pserver_threadfunc(void *threadid)
{
	/**/
	pthread_exit(0);
}

void pserver_loop(struct pserver_config *config)
{
	struct sockaddr_storage ss_peer;
	socklen_t ss_len;
	struct pserver_thread *thread;

	while (1) {
		ss_len = sizeof(struct sockaddr_storage);
		memset(&ss_peer, 0, ss_len);
		if ((config->sd_control = accept(config->sd_listen,
		    (struct sockaddr*)&ss_peer, &ss_len)) == -1) {
		     perror("accept");
		     exit(-1);
		}
		/*Fork out a pserver slave that handles all requests
		  for this control connection. 
		  The parent pserver proc continues accepting*/
		if (fork()) {
			pr_info("Client connected\n");
			while (1) {
			/*TODO: need to a well-defined and generic
			  interface for the control connection.*/
			}
		}
		/*This is wrong, we should fork() for each control
		connection (pserver instance), and the client/server
		negotiates how many threads the test should run*/
		/*
		thread = malloc(sizeof(struct pserver_thread));
		if (!pthread_create(&thread->id, NULL, pserver_threadfunc,
		    NULL)) {
			perror("pthread_create");
			exit(-1);
		}*/
	}
}


int main(int argc, char *argv[])
{

	struct pserver_config config = {{0}};

	LIST_INIT(&pserver_threads);
	if (scan_options(argc, argv, &config))
		return -1;

	if (setup_pserver(&config))
		return -1;

	pserver_loop(&config);
	
	return 0;
}
