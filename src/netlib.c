#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>

void get_addrinfo_byname(char *hostname, char *port, struct addrinfo **ai)
{
	int s;
	struct addrinfo hints = {0};

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if ((s = getaddrinfo(hostname, port, &hints, ai)) != 0) {
		perror("getaddrinfo");
		printf("%s", gai_strerror(s));
		exit(-1);
	}
}
char *print_sockaddr(struct sockaddr *sa, socklen_t salen, char *buf, int buflen)
{
	int s;
	char name[128];
	char port[16];

	if ((s = getnameinfo(sa, salen, name, sizeof(name), port, sizeof(port),
			     NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
		perror("getnameinfo");
		printf("%s", gai_strerror(s));
		exit(-1);
	}
	snprintf(buf, buflen, "%s port %s", name, port);
	return buf;
}

char* print_addrinfo(struct addrinfo *ai, char *buf, int buflen)
{
	return print_sockaddr(ai->ai_addr, ai->ai_addrlen, buf, buflen);
}
