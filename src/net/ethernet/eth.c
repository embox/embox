/**
 * \file eth.c
 * \date Mar 4, 2009
 * \author anton
 */
#include "conio/conio.h"
#include "misc.h"
#include "string.h"
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

void packet_dump(net_packet *);

int eth_init() {
    return 0;
}

int eth_rebuild_header(net_packet *pack) {
    if (NULL == pack) {
        return -1;
    }
    ethhdr     *eth = (ethhdr*)pack->data;
    net_device *dev = pack->netdev;
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

void ether_setup(net_device *dev) {
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

net_device *alloc_etherdev(int num) {
    net_device *dev = alloc_netdev();
    sprintf(dev->name, "eth%d", num);
    ether_setup(dev);
    return dev;
}

int eth_send(net_packet *pack) {
    IF_DEVICE *dev;
    net_device_stats *stats = pack->netdev->get_stats(pack->netdev);

    if ((NULL == pack) || (NULL == pack->ifdev))
        return -1;

    dev = (IF_DEVICE *) pack->ifdev;
    if (ETH_P_ARP != pack->protocol) {
        if (-1 == dev->net_dev->rebuild_header(pack)) {
            net_packet_free(pack);
            stats->tx_err += 1;
            return -1;
        }
    }
//    packet_dump(pack);
    dev->net_dev->hard_start_xmit(pack, pack->netdev);
    /* update statistic */
    stats->tx_packets += 1;
    stats->tx_bytes   += pack->len;
    net_packet_free(pack);
    return 0;
}

/**
 * function must call from net drivers when packet was received
 * and need transmit one throw protocol's stack
 * @param net_packet *pack struct of network packet
 * @return on success, returns 0, on error, -1 is returned
 */
int netif_rx(net_packet *pack) {
    int i;
    IF_DEVICE *dev;
    if ((NULL == pack) || (NULL == pack->netdev)) {
        return -1;
    }
    pack->nh.raw = (void *) pack->data + ETH_HEADER_SIZE;
    if (NULL == (pack->ifdev = ifdev_find_by_name(pack->netdev->name))){
        LOG_ERROR("wrong interface name during receiving packet\n");
        net_packet_free(pack);
        return -1;
    }
    if (ETH_P_ARP == pack->protocol) {
        pack->nh.raw = pack->data + ETH_HEADER_SIZE;
        arp_received_packet(pack);
    }
    if (ETH_P_IP == pack->protocol) {
        pack->nh.raw = pack->data   + ETH_HEADER_SIZE;
        pack->h.raw  = pack->nh.raw + sizeof(iphdr);
        ip_received_packet(pack);
    }
    /* if there are some callback handlers for packet's protocol */
    dev = (IF_DEVICE *) pack->ifdev;
    for (i = 0; i < array_len(dev->cb_info); i++) {
        if (1 == dev->cb_info[i].is_busy) {
            if ((NET_TYPE_ALL_PROTOCOL == dev->cb_info[i].type)
                    || (dev->cb_info[i].type == pack->protocol)) {
                //may be copy pack for different protocols
                dev->cb_info[i].func(pack);
            }
        }
    }
    /* update device statistic */
    net_device_stats *stats = pack->netdev->get_stats(pack->netdev);
    stats->rx_packets += 1;
    stats->rx_bytes   += pack->len;
    //free packet
    net_packet_free(pack);
    return 0;
}

void packet_dump(net_packet *pack) {
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
