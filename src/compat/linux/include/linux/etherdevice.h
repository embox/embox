/**
 * @file
 * @brief
 *
 * @date 09.05.13
 * @author Ilia Vaprol
 */

#ifndef COMPAT_LINUX_LINUX_ETHERDEVICE_H_
#define COMPAT_LINUX_LINUX_ETHERDEVICE_H_

#include <linux/types.h>

/**
 * Determine if the Ethernet address is a multicast.
 * @param addr Pointer to a six-byte array containing the Ethernet address
 * @return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static inline int is_multicast_ether_addr(const void *addr) {
	return (0x01 & *(const __u8 *)addr);
}

/**
 * Determine if the Ethernet address is broadcast
 * @param addr Pointer to a six-byte array containing the Ethernet address
 * @return true if the address is the broadcast address.
 */
static inline int is_broadcast_ether_addr(const void *addr) {
	const __u16 *_addr = addr;
	return (_addr[0] & _addr[1] & _addr[2]) == 0xffff;
}

/**
 * Determine if give Ethernet address is all zeros.
 * @param addr Pointer to a six-byte array containing the Ethernet address
 * @return true if the address is all zeroes.
 */
static inline int is_zero_ether_addr(const void *addr) {
	const __u8 *_addr = addr;
	return !(_addr[0] | _addr[1] | _addr[2] | _addr[3] | _addr[4] | _addr[5]);
}

/**
 * Determine if the given Ethernet address is valid
 * @param addr Pointer to a six-byte array containing the Ethernet address
 *
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 *
 * @return true if the address is valid.
 */
static inline int is_valid_ether_addr(const void *addr) {
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

/**
 * compare_ether_addr - Compare two Ethernet addresses
 * @a: Pointer to a six-byte array containing the Ethernet address
 * @b: Pointer other six-byte array containing the Ethernet address
 *
 * Compare two ethernet addresses, returns 0 if equal
 */
static inline int compare_ether_addr(const void *a, const void *b) {
	const __u16 *_a = a, *_b = b;;
	return (_a[0] ^ _b[0]) | (_a[1] ^ _b[1]) | (_a[2] ^ _b[2]);
}

#endif /* COMPAT_LINUX_LINUX_ETHERDEVICE_H_ */
