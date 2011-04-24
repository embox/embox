/**
 * @file
 * @brief Definitions for the Ethernet handlers.
 * @details IEEE 802.3
 *
 * @date 5.03.2009
 * @author Anton Bondarev
 */
#ifndef ETHERDEVICE_H_
#define ETHERDEVICE_H_

#define NET_TYPE_ALL_PROTOCOL 0

#include <net/netdevice.h>

typedef void (*ETH_LISTEN_CALLBACK)(void * pack);

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
 * Functions provided by eth.c
 */

/**
 * Extract hardware address from packet.
 * @param pack packet to extract header from
 * @param haddr destination buffer
 */
int eth_header_parse(const sk_buff_t *pack, unsigned char *haddr);

/**
 * Set new Ethernet hardware address.
 * @param dev network device
 * @param p socket address
 */
int eth_mac_addr(struct net_device *dev, void *p);

/**
 * Create the Ethernet header
 * @param pack buffer to alter
 * @param dev source device
 * @param type Ethernet type field
 * @param daddr destination address (NULL leave destination address)
 * @param saddr source address (NULL use device source address)
 * @paramlen packet length (<= pack->len)
 */
extern int eth_header(sk_buff_t *pack, net_device_t *dev,
			unsigned short type, void *daddr, void *saddr, unsigned len);

/**
 * Rebuild the Ethernet MAC header.
 * @param pack socket buffer to update
 */
extern int eth_rebuild_header(sk_buff_t *pack);

/**
 * Set new MTU size
 * @param dev network device
 * @param new_mtu new Maximum Transfer Unit
 */
extern int eth_change_mtu(net_device_t *dev, int new_mtu);

/**
 * Setup Ethernet network device
 * @param dev network device
 * Fill in the fields of the device structure with Ethernet-generic values.
 */
extern void ether_setup(net_device_t *dev);

extern const header_ops_t *get_eth_header_ops(void);

/**
 * Allocates and sets up an Ethernet device
 */
extern net_device_t *alloc_etherdev(int sizeof_priv);

/**
 * Determine the packet's protocol ID.
 */
extern __be16 eth_type_trans(struct sk_buff *skb, struct net_device *dev);

extern int eth_flag_up(net_device_t *dev, int flag_type);
extern int eth_flag_down(net_device_t *dev, int flag_type);
extern int eth_set_irq(net_device_t *dev, int irq_num);
extern int eth_set_baseaddr(net_device_t *dev, unsigned long base_addr);
extern int eth_set_txqueuelen(net_device_t *dev, unsigned long new_len);
extern int eth_set_broadcast_addr(net_device_t *dev, unsigned char broadcast_addr[]);

#endif /* ETHERDEVICE_H_ */
