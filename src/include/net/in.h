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
#include <lib/bits/byteswap.h>
#include <net/socket.h>

/**
 * Define info about id of IP protocols
 * @param id - identifer of protocol
 * @return string info
 */

/* IPv4 AF_INET sockets:*/


/* Address to accept any incoming messages. */
#define INADDR_ANY          ((unsigned long int) 0x00000000)

/* Address to send to all hosts. */
#define INADDR_BROADCAST    ((unsigned long int) 0xffffffff)

/* Address indicating an error return. */
#define INADDR_NONE         ((unsigned long int) 0xffffffff)

/* Address to loopback in software to local host.  */
#define INADDR_LOOPBACK     ((unsigned long int) 0x7f000001)   /* 127.0.0.1   */

static inline bool ipv4_is_loopback(in_addr_t addr) {
	/* Loopback address is 127.*.*.* */
	return (addr & htonl(0xff000000)) == htonl(0x7f000000);
}

static inline uint ipv4_mask_len(in_addr_t mask) {
	/* ToDo: reimplement as xor/not, +1 and array map */
	uint m_len = 0;
	while (mask > 0) {
		mask <<= 1;
		m_len++;
	}
	return m_len;
}

static inline bool ipv4_is_multicast(in_addr_t addr) {
	return (addr & htonl(0xf0000000)) == htonl(0xe0000000);
}

#endif /* IN_H_ */
