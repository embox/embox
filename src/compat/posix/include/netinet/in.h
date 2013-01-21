/**
 * @file
 *
 * @brief Internet Protocol family
 *
 * @details http://pubs.opengroup.org/onlinepubs/7908799/xns/netinetin.h.html
 *
 * @date 01.02.2012
 * @author Anton Bondarev
 */

#ifndef NETINET_IN_H_
#define NETINET_IN_H_

#include <types.h>
#include <sys/socket.h>


typedef uint16_t in_port_t; /* An unsigned integral type of exactly 16 bits. */
typedef uint32_t in_addr_t; /* An unsigned integral type of exactly 32 bits. */

/**< Internet address in network representation */
struct in_addr {
	in_addr_t s_addr;
};


/* POSIX descriptions
IPPROTO_IP    Dummy for IP.
IPPROTO_ICMP  Control message protocol.
IPPROTO_TCP   TCP.
IPPROTO_UDP   User datagram protocol.
*/
/**< Standard well-defined IP protocols.  */
enum {
	IPPROTO_IP   = 0,     /* Dummy protocol for TCP            */
	IPPROTO_ICMP = 1,     /* Internet Control Message Protocol */
	IPPROTO_TCP  = 6,     /* Transmission Control Protocol     */
	IPPROTO_UDP  = 17,    /* User Datagram Protocol            */
	/* A protocol of IPPROTO_RAW is able to send any IP protocol
	 * that is specified in the passed header. Receiving of all
	 * IP protocols via IPPROTO_RAW is not possible using raw sockets. */
	IPPROTO_RAW  = 255,   /* Raw IP packets                    */
	IPPROTO_MAX
};


/* more specific address structure
	 (to recast on sockaddr_t in AF_INET sockets) */
typedef struct sockaddr_in {
	sa_family_t      sin_family;   /* e.g. AF_INET */
	uint16_t		 sin_port;     /* e.g. htons(3490) i.e. port in big endian */
	struct in_addr   sin_addr;     /* see struct in_addr, above */
	unsigned char    sin_zero[8];  /* zero this if you want to */
} sockaddr_in_t;


/* Address to accept any incoming messages. */
#define INADDR_ANY          ((unsigned long int) 0x00000000)

/* Address to send to all hosts. */
#define INADDR_BROADCAST    ((unsigned long int) 0xffffffff)

/* Address indicating an error return. */
#define INADDR_NONE         ((unsigned long int) 0xffffffff)

/* Address to loopback in software to local host.  */
#define INADDR_LOOPBACK     ((unsigned long int) 0x7f000001)   /* 127.0.0.1   */


#endif /* NETINET_IN_H_ */
