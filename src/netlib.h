#ifndef _NETLIB_H_
#define _NETLIB_H_

struct addrinfo;

void get_addrinfo_byname(char *hostname, char *port, struct addrinfo **ai);
char *print_addrinfo(struct addrinfo *ai, char *buf, int buflen);
char *print_sockaddr(struct sockaddr *sa, socklen_t salen, char *buf, int buflen);
#endif
