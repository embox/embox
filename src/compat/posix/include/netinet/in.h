/**
 * @file
 * @brief Internet Protocol family
 *
 * @date 01.02.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_NETINET_IN_H_
#define COMPAT_POSIX_NETINET_IN_H_

#include <stdint.h>
#include <sys/socket.h>

typedef uint16_t in_port_t; /* An unsigned integral type of exactly 16 bits. */
typedef uint32_t in_addr_t; /* An unsigned integral type of exactly 32 bits. */

/**
 * IP address
 */
struct in_addr {
	in_addr_t s_addr;
};

/**
 * Socket address for IP protocol
 */
struct sockaddr_in {
	sa_family_t      sin_family;  /* AF_INET */
	in_port_t		 sin_port;    /* Port number (BE) */
	struct in_addr   sin_addr;    /* IP address (BE) */
	unsigned char    sin_zero[8]; /* Padding */
};

/**
 * IPv6 address
 */
struct in6_addr {
	uint8_t s6_addr[16];
};

/**
 * Socket address for IPv6 protocol
 */
struct sockaddr_in6 {
	sa_family_t sin6_family;   /* AF_INET6 */
	in_port_t sin6_port;       /* Port number (BE) */
	uint32_t sin6_flowinfo;    /* Ipv6 traffic class and flow information */
	struct in6_addr sin6_addr; /* Ipv6 address (BE) */
	uint32_t sin6_scope_id;    /* Set of interfaces for a scope */
};

/**
 * Internet protocols
 */
enum {
	IPPROTO_IP   = 0,     /* Internet Protocol */
	IPPROTO_ICMP = 1,     /* Internet Control Message Protocol */
	IPPROTO_TCP  = 6,     /* Transmission Control Protocol */
	IPPROTO_UDP  = 17,    /* User Datagram Protocol */
	IPPROTO_IPV6 = 41,    /* Internet Protocol Version 6 */
	/* A protocol of IPPROTO_RAW is able to send any IP protocol
	 * that is specified in the passed header. Receiving of all
	 * IP protocols via IPPROTO_RAW is not possible using raw sockets. */
	IPPROTO_RAW  = 255,   /* Raw packets */
	IPPROTO_MAX
};


/**
 * IP address constants
 */
#define INADDR_ANY          ((unsigned long int)0x00000000) /* 0.0.0.0 */
#define INADDR_BROADCAST    ((unsigned long int)0xffffffff) /* 255.255.255.255 */
#define INADDR_NONE         ((unsigned long int)0xffffffff) /* 255.255.255.255 */
#define INADDR_LOOPBACK     ((unsigned long int)0x7f000001) /* 127.0.0.1 */

/**
 * IPv6 address constants
 */
//extern const struct in6_addr in6addr_any; /* :: */
#define IN6ADDR_ANY_INIT \
	{ { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } }
//extern const struct in6_addr in6addr_loopback; /* ::1 */
#define IN6ADDR_LOOPBACK_INIT \
	{ { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } } }

/**
 * Maximum length of string representation for IP and IPv6
 */
#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46

/**
 * Ports
 */
#define IPPORT_RESERVED     1024 /* Ports < IPPORT_RESERVED are reserved
									for superuser use */
#define IPPORT_USERRESERVED 5000 /* Ports >= IPPORT_USERRESERVED are
									reserved for explicit use */

#endif /* COMPAT_POSIX_NETINET_IN_H_ */
