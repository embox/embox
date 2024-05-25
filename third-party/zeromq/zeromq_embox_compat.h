/*
 * zeromq_embox_compat.h
 *
 *  Created on: 25 mars 2013
 *      Author: fsulima
 */

#ifndef ZEROMQ_EMBOX_COMPAT_H_
#define ZEROMQ_EMBOX_COMPAT_H_

/*
 *	IPv6 address structure
 */
//
//struct in6_addr {
//	union {
//		unsigned char		u6_addr8[16];
//		unsigned short		u6_addr16[8];
//		unsigned long 		u6_addr32[4];
//	} in6_u;
//#define s6_addr			in6_u.u6_addr8
//#define s6_addr16		in6_u.u6_addr16
//#define s6_addr32		in6_u.u6_addr32
//};
//

/* IPv6 Wildcard Address (::) and Loopback Address (::1) defined in RFC2553
 * NOTE: Be aware the IN6ADDR_* constants and in6addr_* externals are defined
 * in network byte order, not in host byte order as are the IPv4 equivalents
 */
//
//struct sockaddr_in6 {
//	unsigned short int	sin6_family;    /* AF_INET6 */
//	unsigned short sin6_port;      /* Transport layer port # */
//	unsigned long			sin6_flowinfo;  /* IPv6 flow information */
//	struct in6_addr		sin6_addr;      /* IPv6 address */
//	unsigned long			sin6_scope_id;  /* scope id (new in RFC2553) */
//};

#ifdef __cplusplus
extern "C" {
#endif
//	extern const struct sockaddr_in6 in6addr_any;
#ifdef __cplusplus
}
#endif

#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define INADDR_LOOPBACK     ((unsigned long int)0x7f000001) /* 127.0.0.1 */

static inline char *tempnam(const char *dir, const char *pfx) {
	DPRINT();
	return NULL;
}

#define EPROTO          71      /* Protocol error */

#define AI_PASSIVE 0x100
#define AI_NUMERICHOST 0x200

#define EAI_MEMORY 1
#define EAI_FAIL 2

#define NI_MAXHOST 1
#define NI_NUMERICHOST 2

#endif /* ZEROMQ_EMBOX_COMPAT_H_ */
