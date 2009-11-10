/**
 * \file eth.c
 * \date Mar 4, 2009
 * \author anton
 */
#include "conio.h"
#include "misc.h"
#include "string.h"
#include "net/skbuff.h"
#include "net/net_pack_manager.h"
#include "net/net_packet.h"
#include "net/net_device.h"
#include "net/arp.h"
#include "net/udp.h"
#include "net/net.h"
#include "net/eth.h"
#include "net/ip.h"
#include "net/icmp.h"
#include "net/if_device.h"

void packet_dump(sk_buff_type *);

int eth_init() {
    return 0;
}

int eth_rebuild_header(sk_buff_type *pack) {
    if (NULL == pack) {
        return -1;
    }
    ethhdr     *eth = (ethhdr*)pack->data;
    net_device_t *dev = pack->netdev;
    if (NULL == pack->sk || SOCK_RAW != pack->sk->sk_type) {
        if (NULL == arp_resolve_addr(pack, pack->nh.iph->daddr)) {
            LOG_WARN("Destanation host is unreachable\n");
            return -1;
        }
        memcpy(eth->src_addr, dev->hw_addr, sizeof(eth->src_addr));
        eth->type = pack->protocol;
        pack->len += ETH_HEADER_SIZE;
        return 0;
    }
    return 0;
}

void ether_setup(net_device_t *dev) {
    dev->rebuild_header     = eth_rebuild_header;
    /*TODO:
    dev->hard_header        = ...;
    dev->set_mac_address    = ...;*/

    dev->type               = ARPHRD_ETHER;
    dev->addr_len           = ETH_ALEN;
    dev->flags              = IFF_BROADCAST|IFF_MULTICAST;
    dev->irq                = 12;
    dev->base_addr          = 0xCF000000;
    dev->tx_queue_len       = 1000;
    memset(dev->broadcast, 0xFF, ETH_ALEN);
}

net_device_t *alloc_etherdev(int num) {
    net_device_t *dev = alloc_netdev();
    sprintf(dev->name, "eth%d", num);
    ether_setup(dev);
    return dev;
}


/**
 * this function call ip protocol,
 * it call rebuild mac header function, if can resolve dest addr else it send arp packet and drop this packet
 * and send packet by calling hard_start_xmit() function
 * return 0 if success else -1
 */
int eth_send(sk_buff_type *pack) {
    inet_device_t *dev;
    net_device_stats *stats = pack->netdev->get_stats(pack->netdev);

    if ((NULL == pack) || (NULL == pack->ifdev))
        return -1;

    dev = (inet_device_t *) pack->ifdev;
    if (ETH_P_ARP != pack->protocol) {
        if (-1 == dev->net_dev->rebuild_header(pack)) {
            kfree_skb(pack);
            stats->tx_err += 1;
            return -1;
        }
    }
//TODO delete this because we will can manager ifdev debug mod in future
    //packet_dump(pack);
    if (-1 == dev->net_dev->hard_start_xmit(pack, pack->netdev)) {
    	kfree_skb(pack);
    	stats->tx_err += 1;
    	return -1;
    }

    ifdev_tx_callback(pack);

    /* update statistic */
    stats->tx_packets += 1;
    stats->tx_bytes   += pack->len;
    kfree_skb(pack);
    return 0;
}

/**
 * function must call from net drivers when packet was received
 * and need transmit one throw protocol's stack
 * @param net_packet *pack struct of network packet
 * @return on success, returns 0, on error, -1 is returned
 */
int netif_rx(sk_buff_type *pack) {
    int i;
    inet_device_t *dev;
    if ((NULL == pack) || (NULL == pack->netdev)) {
        return -1;
    }
    pack->nh.raw = (void *) pack->data + ETH_HEADER_SIZE;
    if (NULL == (pack->ifdev = inet_dev_find_by_name(pack->netdev->name))){
        LOG_ERROR("wrong interface name during receiving packet\n");
        kfree_skb(pack);
        return -1;
    }

    if (ETH_P_ARP == pack->protocol) {
        arp_received_packet(pack);
    }
    if (ETH_P_IP == pack->protocol) {
        pack->h.raw  = pack->nh.raw + IP_HEADER_SIZE;
        ip_received_packet(pack);
    }

    /* update device statistic */
    net_device_stats *stats = pack->netdev->get_stats(pack->netdev);
    stats->rx_packets += 1;
    stats->rx_bytes   += pack->len;
    //free packet
    kfree_skb(pack);
    return 0;
}

//TODO delete this function from here
void packet_dump(sk_buff_type *pack) {
    char ip[15], mac[18];
    TRACE("--------dump-----------------\n");
    TRACE("protocol=0x%X\n", pack->protocol);
    TRACE("len=%d\n", pack->len);
    TRACE("mac.mach.type=%d\n", pack->mac.ethh->type);
    macaddr_print(mac, pack->mac.ethh->src_addr);
    TRACE("mac.ethh.src_addr=%s\n", mac);
    macaddr_print(mac, pack->mac.ethh->dst_addr);
    TRACE("mac.ethh.dst_addr=%s\n", mac);
    if (pack->protocol == ETH_P_ARP) {
        TRACE("nh.arph.htype=%d\n", pack->nh.arph->htype);
        TRACE("nh.arph.ptype=%d\n", pack->nh.arph->ptype);
        TRACE("nh.arph.hlen=%d\n", pack->nh.arph->hlen);
        TRACE("nh.arph.plen=%d\n", pack->nh.arph->plen);
        TRACE("nh.arph.oper=%d\n", pack->nh.arph->oper);
        macaddr_print(mac, pack->nh.arph->sha);
        TRACE("nh.arph.sha=%s\n", mac);
        ipaddr_print(ip, pack->nh.arph->spa);
        TRACE("nh.arph.spa=%s\n", ip);
        macaddr_print(mac, pack->nh.arph->tha);
        TRACE("nh.arph.tha=%s\n", mac);
        ipaddr_print(ip, pack->nh.arph->tpa);
        TRACE("nh.arph.tpa=%s\n", ip);
    } else if (pack->protocol == ETH_P_IP) {
	TRACE("nh.iph.ihl=%d\n", pack->nh.iph->ihl);
	TRACE("nh.iph.version=%d\n", pack->nh.iph->version);
        TRACE("nh.iph.tos=%d\n", pack->nh.iph->tos);
        TRACE("nh.iph.tot_len=%d\n", pack->nh.iph->tot_len);
        TRACE("nh.iph.id=%d\n", pack->nh.iph->id);
        TRACE("nh.iph.frag_off=%d\n", pack->nh.iph->frag_off);
        TRACE("nh.iph.ttl=%d\n", pack->nh.iph->ttl);
        TRACE("nh.iph.proto=0x%X\n", pack->nh.iph->proto);
        TRACE("nh.iph.check=%d\n", pack->nh.iph->check);
        ipaddr_print(ip, pack->nh.iph->saddr);
        TRACE("nh.iph.saddr=%s\n", ip);
        ipaddr_print(ip, pack->nh.iph->daddr);
        TRACE("nh.iph.daddr=%s\n", ip);
        if (pack->nh.iph->proto == ICMP_PROTO_TYPE) {
            TRACE("h.icmph.type=%d\n", pack->h.icmph->type);
            TRACE("h.icmph.code=%d\n", pack->h.icmph->code);
            TRACE("h.icmph.header_check_summ=%d\n", pack->h.icmph->header_check_summ);
            TRACE("h.icmph.data=0x%X\n", pack->h.icmph->data);
        } else if (pack->nh.iph->proto == UDP_PROTO_TYPE) {
            LOG_DEBUG("h.uh.source=%d\n", pack->h.uh->source);
            LOG_DEBUG("h.uh.dest=%d\n", pack->h.uh->dest);
            LOG_DEBUG("h.uh.len=%d\n", pack->h.uh->len);
            LOG_DEBUG("h.uh.check=%d\n", pack->h.uh->check);
        }
    }
    TRACE("data=0x%X\n", pack->data);
    TRACE("---------------end-----------------\n");
}
