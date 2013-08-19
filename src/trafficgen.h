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

/*
 * UDP header
 */
struct udp_header {
        u_int16_t       sport;
        u_int16_t       dport;
        u_int16_t       len;
        u_int16_t       cksum;
};

typedef struct udp_header udp_hdr;

#define UDP_SIZE (8)

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

typedef struct tcp_header tcp_hdr;

#define TCP_SIZE (20)

