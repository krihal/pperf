#ifndef _NETLIB_H_
#define _NETLIB_H_

/* This struct is sent from the client to request a new
 * testcase.
 */
struct pperf_request {
	uint16_t        ether_proto;
	uint16_t        __PAD;
	uint32_t        ul_proto;
	char            param[64];
};

/* The response message sent back to the client */
struct pperf_response {
	char		param[64];
};

struct addrinfo;

void get_addrinfo_byname(char *hostname, char *port, struct addrinfo **ai);
char *print_addrinfo(struct addrinfo *ai, char *buf, int buflen);
char *print_sockaddr(struct sockaddr *sa, socklen_t salen, char *buf, int buflen);
#endif
