/* Header definitions for the supported traffic types. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <bits/ioctls.h>
#include <net/if.h>
#include <errno.h>

#define UDP_SIZE (8)
#define TCP_SIZE (20)
#define IPV4_SIZE (20)
#define IPv6_SIZE (40)
#define IPv6_ADDRESS_SIZE (16)

/*
 * UDP header
 */
struct udp_header {
        u_int16_t       sport;
        u_int16_t       dport;
        u_int16_t       len;
        u_int16_t       cksum;
};

/*
 * TCP header
 */
struct tcp_header {
        u_int16_t       sport;
        u_int16_t       dport;
        u_int32_t       seq;
        u_int32_t       ackseq;
#if defined(__LITTLE_ENDIAN__)
        u_int16_t       res1:4,doff:4,fin:1,syn:1,rst:1,psh:1, \
                        ack:1,urg:1,ece:1,cwr:1;
#else
        u_int16_t       doff:4,res1:4,cwr:1,ece:1,urg:1,ack:1, \
                        psh:1,rst:1,syn:1,fin:1;
#endif
        u_int16_t       window;
        u_int16_t       cksum;
        u_int16_t       urgptr;
};

/*
 * IPv4 Header
 */
struct ip_header_v4 {
#if defined(__LITTLE_ENDIAN__)
        u_int8_t	hlen:4, ver:4;
#else
        u_int8_t	ver:4, hlen:4;
#endif
	u_int8_t	tos;
	u_int16_t	tlen;
	u_int16_t	id;
	u_int16_t	offs;
	u_int8_t	ttl;
	u_int8_t	proto;
	u_int16_t	cksum;
	u_int32_t	saddr;
	u_int32_t	daddr;
};

struct ip_header_v6 {
#if defined(__LITTLE_ENDIAN__)
        u_int8_t	prioHi:4, ver:4;
#else
        u_int8_t	ver:4, prioHi:4;
#endif
#if defined(__LITTLE_ENDIAN__)
        u_int8_t	zero:4, prioLo:4;
#else
        u_int8_t	prioLo:4, zero:4;
#endif
	u_int16_t	label; 
	u_int16_t	pllen;
	u_int8_t	nexthdr;
	u_int8_t	hoplim;
	u_int8_t	saddr[IPv6_ADDRESS_SIZE];
	u_int8_t	daddr[IPv6_ADDRESS_SIZE];
};

/*
 * Type definitions
 */
typedef struct udp_header udp_hdr;
typedef struct tcp_header tcp_hdr;
typedef struct ip_header_v4 ipv4_hdr;
typedef struct ip_header_v6 ipv6_hdr;


__inline__ void ipv4_swap(ipv4_hdr *ih)
{	
	u_int32_t swap;
	
	swap = ih->daddr;
	
	ih->daddr = ih->saddr;
	ih->saddr = swap;
}

__inline__ void ipv6_swap(ipv6_hdr *ih)
{	
	u_int8_t swap[16];
	
	memcpy(&swap[0], &ih->daddr[0], 16);
	memcpy(&ih->daddr[0], &ih->saddr[0], 16);
	memcpy(&ih->saddr[0], &swap[0], 16);
}
