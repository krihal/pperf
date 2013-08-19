#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

/*
 * Swap source and destination ports before returning packet
 */

void udp_swap_ports(udp_hdr *udp)
{
	u_int16_t swap;

	swap = udp->dport;
	udp->dport = udp->sport;
	udp->sport = swap;
}

void tcp_swap_ports(tcp_hdr *tcp)
{
	u_int16_t swap;

	swap = tcpp->dport;
	tcp->dport = udp->sport;
	tcp->sport = swap;
}

/* 
 * UDP checksum calculator 
 */
u_int32_t checksum(unsigned char *buf, unsigned nbytes, u_int32_t sum)
{
	int i;

	for (i = 0; i < (nbytes & ~1U); i += 2) {
		sum += (u_int16_t)ntohs(*((u_int16_t *)(buf + i)));
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}

	if (i < nbytes) {
		sum += buf[i] << 8;
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}

	return (sum);
}