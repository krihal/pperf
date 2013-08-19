#ifndef _NETLIB_H_
#define _NETLIB_H_


struct pperf_request {
	uint16_t        ether_proto;
	uint16_t        __PAD;
	uint32_t        ul_proto;
	char            test_name[64];
};

struct pperf_option {
	uint32_t	type;
	uint32_t	length;
};



struct addrinfo;

void get_addrinfo_byname(char *hostname, char *port, struct addrinfo **ai);
char *print_addrinfo(struct addrinfo *ai, char *buf, int buflen);
char *print_sockaddr(struct sockaddr *sa, socklen_t salen, char *buf, int buflen);
#endif
