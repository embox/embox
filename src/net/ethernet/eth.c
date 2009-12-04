/**
 * @file eth.c
 *
 * @brief Ethernet-type device handling.
 * @date 4.03.2009
 * @author Anton Bondarev
 */
#include <misc.h>
#include <string.h>
#include <net/skbuff.h>
#include <net/net_pack_manager.h>
#include <net/netdevice.h>
#include <net/arp.h>
#include <net/net.h>
#include <net/etherdevice.h>
#include <net/ip.h>
#include <kernel/module.h>
#include <net/inetdevice.h>
#include <net/sock.h>
#include <net/socket.h>

int __init eth_init() {
	return 0;
}

int eth_header(struct sk_buff *pack, struct net_device *dev, unsigned short type,
            			    void *daddr, void *saddr, unsigned len) {
        ethhdr_t *eth = eth_hdr(pack);

        eth->h_proto = htons(type);
        /*  Set the source hardware address. */
        if (!saddr) {
                saddr = dev->hw_addr;
        }
        memcpy(eth->h_source, saddr, dev->addr_len);

        if (daddr) {
        	memcpy(eth->h_dest, daddr, dev->addr_len);
    		return ETH_HLEN;
	}
        /* Anyway, the loopback-device should never use this function... */
        if (dev->flags & (IFF_LOOPBACK | IFF_NOARP)) {
    		memset(eth->h_dest, 0, dev->addr_len);
    		return ETH_HLEN;
        }
        return -ETH_HLEN;
}

int eth_rebuild_header(sk_buff_t *pack) {
    if (NULL == pack) {
        return -1;
    }
    ethhdr_t     *eth = (ethhdr_t*)pack->data;
    net_device_t *dev = pack->dev;
    if (NULL == pack->sk || SOCK_RAW != pack->sk->sk_type) {
        if (NULL == arp_resolve_addr(pack, pack->nh.iph->daddr)) {
            LOG_WARN("Destanation host is unreachable\n");
            return -1;
        }
        memcpy(eth->h_source, dev->hw_addr, ETH_ALEN);
        eth->h_proto = pack->protocol;
        pack->len += ETH_HEADER_SIZE;
        return 0;
    }
    return 0;
}

/**
 * Extract hardware address from packet.
 * @param pack packet to extract header from
 * @param haddr destination buffer
 */
static int eth_header_parse(const sk_buff_t *pack, unsigned char *haddr) {
	ethhdr_t *eth = eth_hdr(pack);
        memcpy(haddr, eth->h_source, ETH_ALEN);
        return ETH_ALEN;
}

/**
 * Set new Ethernet hardware address.
 * @param dev network device
 * @param p socket address
 */
static int eth_mac_addr(struct net_device *dev, void *p) {
        struct sockaddr *addr = p;
        memcpy(dev->hw_addr, addr->sa_data, dev->addr_len);
        return 0;
}

const struct header_ops eth_header_ops = {
	.rebuild       = eth_rebuild_header,
	.create        = eth_header,
	.parse         = eth_header_parse,
};

void ether_setup(net_device_t *dev) {
	dev->header_ops    = &eth_header_ops;
	dev->type          = ARPHRD_ETHER;
	dev->addr_len      = ETH_ALEN;
	dev->flags         = IFF_BROADCAST|IFF_MULTICAST;
	dev->tx_queue_len  = 1000;
	memset(dev->broadcast, 0xFF, ETH_ALEN);
}

net_device_t *alloc_etherdev() {
	net_device_t *dev = alloc_netdev("eth%d", &ether_setup);
	return dev;
}
