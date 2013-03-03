/*
 * @file
 *
 * @date Nov 21, 2012
 * @author: Anton Bondarev
 */

#ifndef ARPA_INET_H_
#define ARPA_INET_H_

#include <lib/bits/byteswap.h>

#include <netinet/in.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

#define htons(n)            __bswap_16(n)
#define ntohs(n)            __bswap_16(n) /* same as htons() */
#define htonl(n)            __bswap_32(n)
#define ntohl(n)            __bswap_32(n) /* same as htonl() */


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

__END_DECLS


#endif /* ARPA_INET_H_ */
