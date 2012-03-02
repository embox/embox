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

OBJALLOC_DEF(net_packs, struct pnet_pack, CONFIG_PNET_PACKETS_QUANTITY);

#if 0
net_packet_t pnet_pack_alloc_skb(net_node_t node, struct sk_buff *skb) {
	net_packet_t pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir = PNET_PACK_DIRECTION_RX; //TODO varios packet types

	pack->skbuf = skb;

	return pack;
}
#endif
struct pnet_pack *pnet_pack_alloc(net_node_t node, int len) {
	struct pnet_pack *pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir = PNET_PACK_DIRECTION_RX; //TODO varios packet types

	pack->skbuf = alloc_skb(len, 0);

	return pack;
}


int pnet_pack_free(struct pnet_pack *pack) {
	kfree_skb(pack->skbuf);

	objfree(&net_packs, pack);

	return 0;
}
