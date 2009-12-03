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
    net_device_t *dev = pack->netdev;
    if (NULL == pack->sk || SOCK_RAW != pack->sk->sk_type) {
        if (NULL == arp_resolve_addr(pack, pack->nh.iph->daddr)) {
            LOG_WARN("Destanation host is unreachable\n");
            return -1;
        }
        memcpy(eth->h_source, dev->hw_addr, sizeof(eth->h_source));
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
static int eth_header_parse(sk_buff_t *pack, unsigned char *haddr) {
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

void ether_setup(net_device_t *dev) {
    dev->rebuild_header     = eth_rebuild_header;
    dev->hard_header        = eth_header;
    dev->set_mac_address    = eth_mac_addr;

    dev->type               = ARPHRD_ETHER;
    dev->addr_len           = ETH_ALEN;
    dev->flags              = IFF_BROADCAST|IFF_MULTICAST;
    dev->irq                = 12;
    dev->base_addr          = 0xCF000000;
    dev->tx_queue_len       = 1000;
//    dev->broadcast          = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
}

net_device_t *alloc_etherdev(int num) {
    net_device_t *dev = alloc_netdev();
    sprintf(dev->name, "eth%d", num);
    ether_setup(dev);
    return dev;
}
