/**
 * @file
 * @brief
 *
 * @date 29.06.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#ifndef COMPAT_LINUX_LINUX_IN_H_
#define COMPAT_LINUX_LINUX_IN_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>

/**
 * It's loopback address (i.e. 127.*.*.*)
 */
static inline bool ipv4_is_loopback(in_addr_t addr) {
	return (addr & htonl(0xff000000UL)) == htonl(0x7f000000UL);
}

/**
 * It's multicast address
 */
static inline bool ipv4_is_multicast(in_addr_t addr) {
	return (addr & htonl(0xf0000000UL)) == htonl(0xe0000000UL);
}

/**
 * It's broadcast address
 */
static inline bool ipv4_is_broadcast(in_addr_t addr) {
	return addr == htonl(INADDR_BROADCAST);
}


#endif /* COMPAT_LINUX_LINUX_IN_H_ */
