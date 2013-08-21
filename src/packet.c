#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

#include "packet.h"

/* 
 * IP checksum calculator 
 */
unsigned short checksum(unsigned short *ptr, int nbytes) 
{
	register long sum;
	unsigned short oddbyte;
	register short answer;
 
	sum=0;
	while(nbytes>1) {
		sum += *ptr++;
		nbytes -= 2;
	}

	if(nbytes == 1) {
		oddbyte = 0;
		*((u_char*)&oddbyte) =* (u_char*)ptr;
		sum += oddbyte;
	}
	
	sum = (sum >> 16) + (sum & 0xffff);
	sum = sum + (sum >> 16);
	answer = (short) ~sum;
	
	return(answer);
}
 
void ip4_packet(ip_packet_control *ipc, ipv4_hdr *ip)
{
	uint32_t total_length = 0;		
	struct sockaddr_in *sa_dst;
	
	sa_dst = (struct sockaddr_in *)&ipc->dst;

	ip = (ipv4_hdr *)malloc(sizeof(ipv4_hdr));

	ip->hlen = 5;
	ip->ver = 4;
	ip->tos = ipc->tos;
	ip->tlen = total_length;
	ip->id = htonl(1234);
	ip->offs = 0;
	ip->ttl = ipc->ttl;
	ip->proto = ipc->hdr_type;
	ip->cksum = 0;
	ip->saddr = inet_addr(ipc->source_ip);
	ip->daddr = sa_dst->sin_addr.s_addr;
	ip->cksum = checksum((unsigned short *)&ip, ip->hlen);
}

void ip6_packet(ip_packet_control *ipc, ipv6_hdr *ip)
{
}

void udp_packet(ip_packet_control *ipc, udp_hdr *udp)
{
}

void tcp_packet(ip_packet_control *ipc, tcp_hdr *tcp) 
{
	tcp->sport = htons(ipc->src_port);
	tcp->dport = htons(ipc->dst_port);
	tcp->seq = 0;
	tcp->ackseq = 0;
	tcp->doff = 5;
	tcp->fin=0;
	tcp->syn=1;
	tcp->rst=0;
	tcp->psh=0;
	tcp->ack=0;
	tcp->urg=0;
	tcp->ece = htons(5840);
	tcp->cksum = 0;
	tcp->urgptr = 0;
}

void psh(ip_packet_control *ipc, tcp_hdr *tcp, pseudo_header *psh)
{
	struct sockaddr_in *sa_dst;

	sa_dst = (struct sockaddr_in *)&ipc->dst;

	psh->source_address = inet_addr(ipc->source_ip);
	psh->dest_address = sa_dst->sin_addr.s_addr;
	psh->placeholder = 0;
	psh->protocol = IPPROTO_TCP;
	psh->tcp_length = htons(sizeof(struct tcp_header) + ipc->payload_size);
}

char *assemble_ipv4_tcp_packet(ip_packet_control *ipc)
{
	char *packet; /* Jumbo! */
	char *__packet;
	char *__pg;
	ipv4_hdr *__ip;
	tcp_hdr *__tcp;
	pseudo_header __psh;
	int psize = 0;

	packet = malloc(sizeof(char) * 4096);
	memset(packet, 0, 4096);
	
	__ip = (ipv4_hdr *)packet;
	__tcp = (tcp_hdr *)(packet + sizeof(ipv4_hdr));
	__packet = packet + sizeof(ipv4_hdr) + sizeof(tcp_hdr);
	memset(__packet, 1, ipc->payload_size);

	ip4_packet(ipc, __ip);
	tcp_packet(ipc, __tcp);
	psh(ipc, __tcp, &__psh);
	psize = sizeof(pseudo_header) + sizeof(tcp_hdr) + strlen(__packet);

	__pg = malloc(psize);

	memcpy(__pg, (char *)&__psh, sizeof(pseudo_header));
	memcpy(__pg + sizeof(pseudo_header), __tcp, sizeof(tcp_hdr) + strlen(packet));

	__tcp->cksum = checksum((unsigned short*)__pg, psize);
	
	return packet;
}
