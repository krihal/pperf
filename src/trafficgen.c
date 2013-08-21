#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "trafficgen.h"

/* 
 * UDP checksum calculator 
 */
unsigned short ip_checksum(unsigned short *ptr, int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;
 
	sum=0;
	while(nbytes>1) {
		sum+=*ptr++;
		nbytes-=2;
	}

	if(nbytes==1) {
		oddbyte=0;
		*((u_char*)&oddbyte)=*(u_char*)ptr;
		sum+=oddbyte;
	}
	
	sum = (sum>>16)+(sum & 0xffff);
	sum = sum + (sum>>16);
	answer=(short)~sum;
	
	return(answer);
}
 
ipv4_hdr ip4_packet(ip_packet_control *ipc)
{
	ipv4_hdr ip;
	uint32_t total_length = 0;		
	struct sockaddr_storage ss_dst;
	struct sockaddr_in *sa_dst;

	ss_dst = ipc->dst;
	sa_dst = (struct sockaddr_in *)&ss_dst;

	ip.hlen = 5;
	ip.ver = 4;
	ip.tos = ipc->tos;
	ip.tlen = total_length;
	ip.id = htonl(1234);
	ip.offs = 0;
	ip.ttl = ipc->ttl;
	ip.proto = ipc->hdr_type;
	ip.cksum = 0;
	ip.saddr = inet_addr(ipc->source_ip);
	ip.daddr = sa_dst->sin_addr.s_addr;
	ip.cksum = ip_checksum((unsigned short *)&ip, ip.hlen);

	return ip;

}

ipv6_hdr ip6_packet(ip_packet_control *ipc)
{
	ipv6_hdr ip;

	return ip;
}

char *packet_udp()
{
	return NULL;
}

char *packet_tcp() 
{
	return NULL;
}
