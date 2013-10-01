/*
 * @file
 * @brief
 *
 * @date 21.11.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef COMPAT_POSIX_ARPA_INET_H_
#define COMPAT_POSIX_ARPA_INET_H_

#include <hal/arch.h>
#include <swab.h>
#include <netinet/in.h>
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <endian.h>

__BEGIN_DECLS

/**
 * Convert values between host and network byte order
 */
#if __BYTE_ORDER == __LITTLE_ENDIAN
# define htons(n) swab16(n)
# define ntohs(n) swab16(n)
# define htonl(n) swab32(n)
# define ntohl(n) swab32(n)
#elif  __BYTE_ORDER == __BIG_ENDIAN
# define htons(n) (n)
# define ntohs(n) (n)
# define htonl(n) (n)
# define ntohl(n) (n)
#else
#error
#endif
/**
 * IPv4 address manipulation
 */
extern in_addr_t inet_addr(const char *cp);
extern char * inet_ntoa(struct in_addr in);
extern int inet_aton(const char *cp, struct in_addr *addr);

/**
 * Convert IPv4 and IPv6 addresses between binary and text form
 */
extern const char * inet_ntop(int af, const void *src, char *dst, socklen_t size);
extern int inet_pton(int af, const char *src, void *dst);

__END_DECLS

#endif /* COMPAT_POSIX_ARPA_INET_H_ */
