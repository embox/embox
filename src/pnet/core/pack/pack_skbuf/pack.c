/**
 * @file
 * @brief Network packet
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <string.h>

#include <net/skbuff.h>
#include <pnet/core.h>

#include <mem/objalloc.h>

#include <embox/unit.h>

#define NET_PACKS_CNT CONFIG_PNET_PACKETS_QUANTITY

OBJALLOC_DEF(net_packs, struct net_packet, NET_PACKS_CNT);

net_packet_t pnet_pack_alloc_skb(net_node_t node, struct sk_buff *skb) {
	net_packet_t pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir = NET_PACKET_RX; //TODO varios packet types

	pack->skbuf = skb;

	return pack;
}

net_packet_t pnet_pack_alloc(net_node_t node, int len) {
	net_packet_t pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir = NET_PACKET_RX; //TODO varios packet types

	pack->skbuf = alloc_skb(len, 0);

	return pack;
}


int pnet_pack_free(net_packet_t pack) {
	kfree_skb(pack->skbuf);

	objfree(&net_packs, pack);

	return 0;
}
