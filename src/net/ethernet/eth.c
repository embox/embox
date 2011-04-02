/**
 * @file
 * @brief Ethernet-type device handling.
 *
 * @date 4.03.09
 * @author Anton Bondarev
 */

#include <string.h>
#include <errno.h>

#include <netutils.h>
#include <net/netdevice.h>
#include <net/arp.h>
#include <net/ip.h>
#include <net/inetdevice.h>
#include <net/sock.h>

int eth_header(sk_buff_t *pack, net_device_t *dev, unsigned short type,
			void *daddr, void *saddr, unsigned len) {
	ethhdr_t *eth = eth_hdr(pack);
	eth->h_proto = htons(type);
	/*  Set the source hardware address. */
	if (!saddr) {
		saddr = dev->dev_addr;
	}
	memcpy(eth->h_source, saddr, ETH_ALEN);

	if (daddr) {
		memcpy(eth->h_dest, daddr, ETH_ALEN);
		return ETH_HLEN;
	}
	/* Anyway, the loopback-device should never use this function... */
	if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
		memset(eth->h_dest, 0, ETH_ALEN);
		return ETH_HLEN;
	}
	return -ETH_HLEN;
}

int eth_rebuild_header(sk_buff_t *pack) {
	ethhdr_t     *eth = (ethhdr_t*) pack->data;
	net_device_t *dev = pack->dev;
	eth->h_proto = pack->protocol;

	if (eth->h_proto == htons(ETH_P_IP)) {
		memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);
		return arp_find(eth->h_dest, pack);
	} else {
		LOG_WARN("%s: unable to resolve type %X addresses.\n",
					dev->name, (int)eth->h_proto);
	}
	return 0;
}

int eth_header_parse(const sk_buff_t *pack, unsigned char *haddr) {
	const ethhdr_t *eth = eth_hdr(pack);
	memcpy(haddr, eth->h_source, ETH_ALEN);
	return ETH_ALEN;
}

int eth_mac_addr(net_device_t *dev, void *p) {
	struct sockaddr *addr = p;
	if (!is_valid_ether_addr((const uint8_t *) addr->sa_data)) {
		return -EADDRNOTAVAIL;
	}
	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	return 0;
}

int eth_change_mtu(net_device_t *dev, int new_mtu) {
	if (new_mtu < 68 || new_mtu > ETH_DATA_LEN)
		return -EINVAL;
	dev->mtu = new_mtu;
	return 0;
}

int eth_flag_up(net_device_t *dev, int flag_type) {
	dev->flags |= flag_type;
	return 0;
}

int eth_flag_down(net_device_t *dev, int flag_type) {
	dev->flags &= ~flag_type;
	return 0;
}

int eth_set_irq(net_device_t *dev, int irq_num) {
	dev->irq = irq_num;
	return 0;
}

int eth_set_baseaddr(net_device_t *dev, unsigned long base_addr) {
	dev->base_addr = base_addr;
	return 0;
}

int eth_set_txqueuelen(net_device_t *dev, unsigned long new_len) {
	dev->tx_queue_len = new_len;
	return 0;
}

int eth_set_broadcast_addr(net_device_t *dev, unsigned char broadcast_addr[]) {
	strncpy((char *) dev->broadcast,
		(const char *) broadcast_addr, sizeof(dev->broadcast));
	LOG_ERROR("not realized\n");
	return 0;
}

const struct header_ops eth_header_ops = {
	.create        = eth_header,
	.parse         = eth_header_parse,
	.rebuild       = eth_rebuild_header,
};

header_ops_t *get_eth_header_ops() {
	return &eth_header_ops;
}

void ether_setup(net_device_t *dev) {
	dev->header_ops    = &eth_header_ops;
	dev->type          = ARPHRD_ETHER;
	dev->mtu           = ETH_DATA_LEN;
	dev->addr_len      = ETH_ALEN;
	dev->flags         = IFF_BROADCAST|IFF_MULTICAST;
	dev->tx_queue_len  = 1000;
	memset(dev->broadcast, 0xFF, ETH_ALEN);
}

net_device_t *alloc_etherdev(int sizeof_priv) {
	return alloc_netdev(sizeof_priv, "eth%d", ether_setup);
}

__be16 eth_type_trans(struct sk_buff *skb, struct net_device *dev) {
	return skb->mac.ethh->h_proto;
}
