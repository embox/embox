/**
 * @file
 *
 * @date 29.06.09
 * @author Nikolay Korotky
 */
/*TODO: move out of here.*/

#ifndef IN_H_
#define IN_H_

#include <types.h>
//#include <embox/kernel.h>
#include <lib/bits/byteswap.h>

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

/**
 * Define info about id of IP protocols
 * @param id - identifer of protocol
 * @return string info
 */
static inline char* trace_proto_info(int id) {
	switch(id) {
		case IPPROTO_IP:
			return "Dummy protocol for TCP";
		case IPPROTO_ICMP:
			return "Internet Control Message Protocol";
		case IPPROTO_TCP:
			return "Transmission Control Protocol";
		case IPPROTO_UDP:
			return "User Datagram Protocol";
		case IPPROTO_RAW:
			return "RAW";
	}
	return "";
}

/* IPv4 AF_INET sockets:*/

typedef uint32_t in_addr_t;

/**< Internet address. */
struct in_addr {
	uint32_t    s_addr;
};

struct sockaddr_in {
	short            sin_family;   /* e.g. AF_INET */
	unsigned short   sin_port;     /* e.g. htons(3490) */
	struct in_addr   sin_addr;     /* see struct in_addr, below */
	char             sin_zero[8];  /* zero this if you want to */
};

/**
 * Convert Internet number in IN to ASCII representation.
 * The return value is a pointer to an internal array containing the string.
 */
char *inet_ntoa(struct in_addr in);

/**
 * Convert Internet host address from numbers-and-dots notation in CP
 * into binary data in network byte order.
 */
in_addr_t inet_addr(const char *cp);

/**
 * Convert Internet host address from numbers-and-dots notation in CP
 * into binary data and store the result in the structure INP.
 */
int inet_aton(const char *cp, struct in_addr *addr);

/*TODO: htons not realize now*/
#define htons(n)            __bswap_16(n)
#define ntohs(n)            n
#define htonl(n)            __bswap_32(n)
#define ntohl(n)            n

/* Address to accept any incoming messages. */
#define INADDR_ANY          ((unsigned long int) 0x00000000)

/* Address to send to all hosts. */
#define INADDR_BROADCAST    ((unsigned long int) 0xffffffff)

/* Address indicating an error return. */
#define INADDR_NONE         ((unsigned long int) 0xffffffff)

/* Address to loopback in software to local host.  */
#define INADDR_LOOPBACK     ((unsigned long int) 0x7f000001)   /* 127.0.0.1   */

static inline bool ipv4_is_loopback(in_addr_t addr) {
	return (addr & htonl(0xff000000)) == htonl(0x7f000000);
}

static inline bool ipv4_is_multicast(in_addr_t addr) {
	return (addr & htonl(0xf0000000)) == htonl(0xe0000000);
}

#endif /* IN_H_ */
