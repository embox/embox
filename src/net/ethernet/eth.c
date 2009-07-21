/**
 * \file eth.c
 * \date Mar 4, 2009
 * \author anton
 */
#include "misc.h"
#include "string.h"
#include "core/net_device.h"
#include "core/net_pack_manager.h"
#include "ipv4/arp.h"
#include "ipv4/udp.h"
#include "net.h"
#include "eth.h"
#include "if_device.h"





void packet_dump(net_packet *);

int eth_init() {
    return 0;
}

/**
 * send packet into define eth interface
 * @param network packet which want send
 */
int eth_send(net_packet *pack) {
    IF_DEVICE *dev = (IF_DEVICE *) pack->ifdev;

    if ((NULL == pack) || (NULL == pack->ifdev))
        return -1;

    if (ARP_PROTOCOL_TYPE != pack->protocol) {
        if (-1 == dev->net_dev->rebuild_header(pack)) {
            net_packet_free(pack);
            return -1;
        }
    }
    //	packet_dump(pack);
    dev->net_dev->hard_start_xmit(pack, pack->netdev);
    net_packet_free(pack);
    return 0;
}


/**
 * function must call from net drivers when packet was received
 * and need transmit one throw protool's stack
 * @param net_packet *pack struct of network packet
 */
int netif_rx(net_packet *pack) {
    int i;

    IF_DEVICE *dev;
    if ((NULL == pack) || (NULL == pack->netdev)) {
        return -1;
    }
    pack->nh.raw = (void *) pack->data + sizeof(machdr);

    if (NULL == (pack->ifdev = ifdev_find_by_name(pack->netdev->name))){
        TRACE("Error: wrong interface name during receiving packet\n");
        net_packet_free(pack);
        return 0;
    }
    if (ARP_PROTOCOL_TYPE == pack->protocol) {
        pack->nh.raw = pack->data + sizeof(machdr);
        arp_received_packet(pack);
    }

    if (IP_PROTOCOL_TYPE == pack->protocol) {
        pack->nh.raw = pack->data + sizeof(machdr);
        pack->h.raw = pack->nh.raw + sizeof(iphdr);
        ip_received_packet(pack);
    }

    //if there are some callback handlers for packet's protocol
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
    //free packet
    net_packet_free(pack);
    return 0;
}



void packet_dump(net_packet *pack) {
    char ip[15], mac[18];
    LOG_DEBUG("--------dump-----------------\n");
    LOG_DEBUG("protocol=0x%X\n", pack->protocol);
    LOG_DEBUG("len=%d\n", pack->len);
    LOG_DEBUG("mac.mach.type=%d\n", pack->mac.mach->type);
    macaddr_print(mac, pack->mac.mach->src_addr);
    LOG_DEBUG("mac.mach.src_addr=%s\n", mac);
    macaddr_print(mac, pack->mac.mach->dst_addr);
    LOG_DEBUG("mac.mach.dst_addr=%s\n", mac);
    if (pack->protocol == ARP_PROTOCOL_TYPE) {
        LOG_DEBUG("nh.arph.htype=%d\n", pack->nh.arph->htype);
        LOG_DEBUG("nh.arph.ptype=%d\n", pack->nh.arph->ptype);
        LOG_DEBUG("nh.arph.hlen=%d\n", pack->nh.arph->hlen);
        LOG_DEBUG("nh.arph.plen=%d\n", pack->nh.arph->plen);
        LOG_DEBUG("nh.arph.oper=%d\n", pack->nh.arph->oper);
        macaddr_print(mac, pack->nh.arph->sha);
        LOG_DEBUG("nh.arph.sha=%s\n", mac);
        ipaddr_print(ip, pack->nh.arph->spa);
        LOG_DEBUG("nh.arph.spa=%s\n", ip);
        macaddr_print(mac, pack->nh.arph->tha);
        LOG_DEBUG("nh.arph.tha=%s\n", mac);
        ipaddr_print(ip, pack->nh.arph->tpa);
        LOG_DEBUG("nh.arph.tpa=%s\n", ip);
    } else if (pack->protocol == IP_PROTOCOL_TYPE) {
        LOG_DEBUG("nh.iph.tos=%d\n", pack->nh.iph->tos);
        LOG_DEBUG("nh.iph.tot_len=%d\n", pack->nh.iph->tot_len);
        LOG_DEBUG("nh.iph.id=%d\n", pack->nh.iph->id);
        LOG_DEBUG("nh.iph.frag_off=%d\n", pack->nh.iph->frag_off);
        LOG_DEBUG("nh.iph.ttl=%d\n", pack->nh.iph->ttl);
        LOG_DEBUG("nh.iph.proto=0x%X\n", pack->nh.iph->proto);
        LOG_DEBUG("nh.iph.check=%d\n", pack->nh.iph->check);
        ipaddr_print(ip, pack->nh.iph->saddr);
        LOG_DEBUG("nh.iph.saddr=%s\n", ip);
        ipaddr_print(ip, pack->nh.iph->daddr);
        LOG_DEBUG("nh.iph.daddr=%s\n", ip);
        if (pack->nh.iph->proto == ICMP_PROTO_TYPE) {
            LOG_DEBUG("h.icmph.type=%d\n", pack->h.icmph->type);
            LOG_DEBUG("h.icmph.code=%d\n", pack->h.icmph->code);
            LOG_DEBUG("h.icmph.header_check_summ=%d\n", pack->h.icmph->header_check_summ);
            LOG_DEBUG("h.icmph.data=0x%X\n", pack->h.icmph->data);
        } else if (pack->nh.iph->proto == UDP_PROTO_TYPE) {
            LOG_DEBUG("h.uh.source=%d\n", pack->h.uh->source);
            LOG_DEBUG("h.uh.dest=%d\n", pack->h.uh->dest);
            LOG_DEBUG("h.uh.len=%d\n", pack->h.uh->len);
            LOG_DEBUG("h.uh.check=%d\n", pack->h.uh->check);
        }
    }
    LOG_DEBUG("data=0x%X\n", pack->data);
    LOG_DEBUG("---------------end-----------------\n");
}
