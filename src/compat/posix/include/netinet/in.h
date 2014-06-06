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
	union {
		uint8_t s_addr8[4];
		uint16_t s_addr16[2];
		in_addr_t s_addr;
	};
};

/**
 * Socket address for IP protocol
 */
struct sockaddr_in {
	sa_family_t      sin_family;  /* AF_INET */
	in_port_t		 sin_port;    /* Port number (BE) */
	struct in_addr   sin_addr;    /* IP address (BE) */
};

/**
 * IPv6 address
 */
struct in6_addr {
	union {
		uint8_t s6_addr[16];
		uint16_t s6_addr16[8];
		uint32_t s6_addr32[4];
	};
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
	IPPROTO_ICMPV6 = 58,  /* ICMPv6 */
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
#define INADDR_LOOPBACK     ((unsigned long int)0x7f000001) /* 127.0.0.1 */
#define INADDR_BROADCAST    ((unsigned long int)0xffffffff) /* 255.255.255.255 */

/**
 * IPv6 address constants
 */
extern const struct in6_addr in6addr_any; /* :: */
#define IN6ADDR_ANY_INIT \
	{ { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } }
extern const struct in6_addr in6addr_loopback; /* ::1 */
#define IN6ADDR_LOOPBACK_INIT \
	{ { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } } }

/**
 * Maximum length of string representation for IP and IPv6
 */
#define INET_ADDRSTRLEN  16
#define INET6_ADDRSTRLEN 46

/**
 * Test for special IPv6 adresses
 */
#define IN6_IS_ADDR_UNSPECIFIED(a) \
	({ \
	 	const struct in6_addr *__a = (const struct in6_addr *)(a); \
	 	(__a->s6_addr32[0] == 0) && (__a->s6_addr32[1] == 0) \
	 			&& (__a->s6_addr32[2] == 0) && (__a->s6_addr32[3] == 0); \
	})
#define IN6_IS_ADDR_LOOPBACK(a) \
	({ \
	 	const struct in6_addr *__a = (const struct in6_addr *)(a); \
	 	(__a->s6_addr32[0] == 0) && (__a->s6_addr32[1] == 0) \
	 			&& (__a->s6_addr32[2] == 0) && (__a->s6_addr32[3] == htonl(1)); \
	})
#define IN6_IS_ADDR_V4MAPPED(a) \
	({ \
		const struct in6_addr *__a = (const struct in6_addr *)(a); \
		(__a->s6_addr32[0] == 0) && (__a->s6_addr32[1] == 0) \
				&& (__a->s6_addr32[2] == htonl(0xffff)); \
	})
#define IN6_IS_ADDR_V4COMPAT(a) \
	({ \
		const struct in6_addr *__a = (const struct in6_addr *)(a); \
		(__a->s6_addr32[0] == 0) && (__a->s6_addr32[1] == 0) \
				&& (__a->s6_addr32[2] == 0) && (htonl(__a->s6_addr32[3]) > 1); \
	})
#define IN6_IS_ADDR_MULTICAST(a) \
	(((const struct in6_addr *)(a))->s6_addr == 0xFF)

/**
 * Ports
 */
#define IPPORT_RESERVED     1024 /* Ports < IPPORT_RESERVED are reserved
									for superuser use */
#define IPPORT_USERRESERVED 5000 /* Ports >= IPPORT_USERRESERVED are
									reserved for explicit use */

#include <arpa/inet.h>

#endif /* COMPAT_POSIX_NETINET_IN_H_ */
