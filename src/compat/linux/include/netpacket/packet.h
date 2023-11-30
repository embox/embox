#ifndef INCLUDE_NETPACKET_PACKET_H_
#define INCLUDE_NETPACKET_PACKET_H_

#include <sys/socket.h>
#include <net/if_packet.h>

#define SO_ATTACH_FILTER 26

struct sockaddr_ll {
	unsigned short sll_family;   /* Always AF_PACKET */
	unsigned short sll_protocol; /* Physical layer protocol */
	int            sll_ifindex;  /* Interface number */
	unsigned short sll_hatype;   /* ARP hardware type */
	unsigned char  sll_pkttype;  /* Packet type */
	unsigned char  sll_halen;    /* Length of address */
	unsigned char  sll_addr[8];  /* Physical layer address */
};

struct sock_filter {	/* Filter block */
	__u16	code;   /* Actual filter code */
	__u8	jt;	/* Jump true */
	__u8	jf;	/* Jump false */
	__u32	k;      /* Generic multiuse field */
};

struct sock_fprog {
	unsigned short len;
	struct sock_filter *filter;
};

#endif /* INCLUDE_NETPACKET_PACKET_H_ */
