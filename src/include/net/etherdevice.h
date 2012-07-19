/**
 * @file
 * @brief Definitions for the Ethernet handlers.
 * @details IEEE 802.3
 *
 * @date 5.03.09
 * @author Anton Bondarev
 */

#ifndef NET_ETHERDEVICE_H_
#define NET_ETHERDEVICE_H_

//#define NET_TYPE_ALL_PROTOCOL 0

#include <net/netdevice.h>
#include <net/skbuff.h>
#include <net/socket.h>
#include <stdint.h>

/**
 * Determine if the Ethernet address is a multicast.
 * @param addr Pointer to a six-byte array containing the Ethernet address
 * @return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static inline int is_multicast_ether_addr(const uint8_t *addr) {
	return (0x01 & addr[0]);
}

/**
 * Determine if the Ethernet address is broadcast
 * @param addr Pointer to a six-byte array containing the Ethernet address
 * @return true if the address is the broadcast address.
 */
static inline int is_broadcast_ether_addr(const uint8_t *addr) {
	return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

/**
 * Determine if give Ethernet address is all zeros.
 * @param addr Pointer to a six-byte array containing the Ethernet address
 * @return true if the address is all zeroes.
 */
static inline int is_zero_ether_addr(const uint8_t *addr) {
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
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
static inline int is_valid_ether_addr(const uint8_t *addr) {
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
static inline bool compare_ether_addr(const uint8_t *a, const uint8_t *b)
{
	return ( (a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2]) |
			 (a[3] ^ b[3]) | (a[4] ^ b[4]) | (a[5] ^ b[5]) );
}

/**
 * Packet types.
 */
enum {
	PACKET_HOST,      /* To us */
	PACKET_BROADCAST, /* To all */
	PACKET_MULTICAST, /* To group */
	PACKET_OTHERHOST, /* To someone else */
	PACKET_LOOPBACK   /* We are in loopback, it overwrites everything */
};

/**
 * Functions provided by eth.c
 */

/**
 * get_eth_header_ops - returns methods for working with ethernet header
 */
extern const struct header_ops * get_eth_header_ops(void);

/**
 * eth_packet_type - determine the packet type (See above)
 * @param skb: skb holding incoming packet
 * (at least LL info and incoming device)
 */
extern uint8_t eth_packet_type(struct sk_buff *skb);

/**
 * Determine the packet's protocol ID.
 */
extern __be16 eth_type_trans(struct sk_buff *skb, struct net_device *dev);

/**
 * Functions provided by eth_dev.c
 */

/**
 * Set new Ethernet hardware address.
 * @param dev network device
 * @param addr socket address
 */
extern int eth_mac_addr(struct net_device *dev, struct sockaddr *addr);

/**
 * Set new MTU size
 * @param dev network device
 * @param new_mtu new Maximum Transfer Unit
 */
extern int eth_change_mtu(struct net_device *dev, int new_mtu);

#if 0
/**
 * Setup Ethernet network device
 * @param dev network device
 * Fill in the fields of the device structure with Ethernet-generic values.
 */
extern void etherdev_setup(struct net_device *dev);
#endif

/**
 * Allocates and sets up an Ethernet device
 */
extern struct net_device * alloc_etherdev(void/*int sizeof_priv*/);

/**
 * Frees an Ethernet device
 */
extern void free_etherdev(struct net_device *dev);

extern int eth_flag_up(struct net_device *dev, int flag_type);
extern int eth_flag_down(struct net_device *dev, int flag_type);
extern int eth_set_irq(struct net_device *dev, int irq_num);
extern int eth_set_baseaddr(struct net_device *dev, unsigned long base_addr);
extern int eth_set_txqueuelen(struct net_device *dev, unsigned long new_len);
extern int eth_set_broadcast_addr(struct net_device *dev, unsigned char broadcast_addr[]);

#endif /* NET_ETHERDEVICE_H_ */
