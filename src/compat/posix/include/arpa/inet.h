/*
 * @file
 * @brief
 *
 * @date 21.11.12
 * @author Anton Bondarev
 */

#ifndef COMPAT_POSIX_ARPA_INET_H_
#define COMPAT_POSIX_ARPA_INET_H_

#include <hal/arch.h>
#include <linux/swab.h>
#include <netinet/in.h>
#include <sys/cdefs.h>

__BEGIN_DECLS

#if defined(__LITTLE_ENDIAN)
# define htons(n) swab16(n)
# define ntohs(n) swab16(n)
# define htonl(n) swab32(n)
# define ntohl(n) swab32(n)
#elif defined (__BIG_ENDIAN)
# define htons(n) (n)
# define ntohs(n) (n)
# define htonl(n) (n)
# define ntohl(n) (n)
#endif


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


#endif /* COMPAT_POSIX_ARPA_INET_H_ */
