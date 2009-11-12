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

void packet_dump(sk_buff_t *);

int eth_init() {
    return 0;
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






//TODO delete this function from here
void packet_dump(sk_buff_t *pack) {
    char ip[15], mac[18];
    TRACE("--------dump-----------------\n");
    TRACE("protocol=0x%X\n", pack->protocol);
    TRACE("len=%d\n", pack->len);
    TRACE("mac.mach.type=%d\n", pack->mac.ethh->h_proto);
    macaddr_print(mac, pack->mac.ethh->h_source);
    TRACE("mac.ethh.src_addr=%s\n", mac);
    macaddr_print(mac, pack->mac.ethh->h_dest);
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
