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
#include <pthread.h>

#include "netlib.h"

#define PSERVER_DEFAULT_LISTEN_PORT "4999"
#define PSERVER_DEFAULT_LISTEN_ADDR "::"

struct pserver_config {
	struct addrinfo *ai_listen;
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

#ifndef pr_info
#define pr_info(fmt, ...) {if(verbosity >= 1) \
				printf("info: " fmt, __VA_ARGS__);}
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
	const char on = 1;
	char buf[128];

	config->sd_listen = socket(config->ai_listen->ai_family,
				 config->ai_listen->ai_socktype,
				 config->ai_listen->ai_protocol);
	setsockopt(config->sd_listen, SOL_SOCKET, SO_REUSEADDR, &on,
		   sizeof(on));
	bind(config->sd_listen, (struct sockaddr*)config->ai_listen->ai_addr,
	     config->ai_listen->ai_addrlen);
	/*TODO: error checking..*/
	listen(config->sd_listen, 10);
	pr_info("Pserver listening on %s\n", print_addrinfo(config->ai_listen, buf, sizeof(buf)));
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
	struct sockaddr *sa = (struct sockaddr*)&ss_peer;
	socklen_t ss_len;
	char buf[128];

	while (1) {
		ss_len = sizeof(struct sockaddr_storage);
		memset(&ss_peer, 0, ss_len);
		if ((config->sd_control = accept(config->sd_listen, sa, &ss_len)) == -1) {
		     perror("accept");
		     exit(-1);
		}
		/*Fork out a pserver slave that handles all requests
		  for this control connection. 
		  The parent pserver proc continues accepting*/
		if (fork()) {
			pr_info("Client %s connected\n", print_sockaddr(sa, ss_len, buf, sizeof(buf)));
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

	struct pserver_config config = {0};

	LIST_INIT(&pserver_threads);
	if (scan_options(argc, argv, &config))
		return -1;

	if (setup_pserver(&config))
		return -1;

	pserver_loop(&config);
	
	return 0;
}
