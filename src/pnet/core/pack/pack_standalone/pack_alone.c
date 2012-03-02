/**
 * @file
 * @brief Network packet
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <string.h>

#include <mem/objalloc.h>

#include <pnet/core.h>


#define NET_PACKS_CNT CONFIG_PNET_PACKETS_QUANTITY
#define PACK_DATA_LEN 64 //TODO

OBJALLOC_DEF(net_packs, struct net_packet, NET_PACKS_CNT);
OBJALLOC_DEF(net_packs_data, unsigned char[PACK_DATA_LEN], NET_PACKS_CNT);

struct net_packet *pnet_pack_alloc(net_node_t node, int len) {
	net_packet_t pack;

	if (len > PACK_DATA_LEN) {
		return NULL;
	}

	pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir = NET_PACKET_RX; //TODO varios packet types

	pack->data = objalloc(&net_packs_data);

	pack->len = len;

	return pack;
}

int pnet_pack_free(struct net_packet *pack) {
	objfree(&net_packs_data, pack->data);

	objfree(&net_packs, pack);

	return 0;
}
