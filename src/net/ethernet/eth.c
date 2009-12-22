/**
 * @file eth.c
 *
 * @brief Ethernet-type device handling.
 * @date 4.03.09
 * @author Anton Bondarev
 */
#include <misc.h>
#include <string.h>
#include <codes.h>
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
        ethhdr_t     *eth = (ethhdr_t*)pack->data;
        net_device_t *dev = pack->dev;
	eth->h_proto = pack->protocol;

        if(eth->h_proto == htons(ETH_P_IP)) {
                return arp_find(eth->h_dest, pack);
        } else {
    		LOG_WARN("%s: unable to resolve type %X addresses.\n",
                                dev->name, (int)eth->h_proto);
        	memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);
        }
#if 0
        if (NULL == pack->sk || SOCK_RAW != pack->sk->sk_type) {
                if (NULL == arp_find(pack, pack->nh.iph->daddr)) {
                        LOG_WARN("Destanation host is unreachable\n");
                        return -1;
                }
                memcpy(eth->h_source, dev->dev_addr, ETH_ALEN);
                eth->h_proto = pack->protocol;
                pack->len += ETH_HEADER_SIZE;
                return 0;
        }
#endif
        return 0;
}

int eth_header_parse(const sk_buff_t *pack, unsigned char *haddr) {
        const ethhdr_t *eth = eth_hdr(pack);
        memcpy(haddr, eth->h_source, ETH_ALEN);
        return ETH_ALEN;
}

int eth_mac_addr(net_device_t *dev, void *p) {
        struct sockaddr *addr = p;
        if (!is_valid_ether_addr(addr->sa_data)) {
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

const struct header_ops eth_header_ops = {
        .create        = eth_header,
        .parse         = eth_header_parse,
        .rebuild       = eth_rebuild_header,
};

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
