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

#include <pnet/pnet_pack.h>

#define PACK_DATA_LEN 64 //TODO

OBJALLOC_DEF(net_packs, struct net_packet, CONFIG_PNET_PACKETS_QUANTITY);
OBJALLOC_DEF(net_packs_data, unsigned char[PACK_DATA_LEN], CONFIG_PNET_PACKETS_QUANTITY);

struct net_packet *pnet_pack_alloc(net_node_t node, void *data, size_t len) {
	net_packet_t pack;

	if (len > PACK_DATA_LEN) {
		return NULL;
	}

	pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir = PNET_PACK_DIRECTION_RX; //TODO varios packet types

	pack->data = objalloc(&net_packs_data);

	pack->len = len;

	return pack;
}

int pnet_pack_free(struct net_packet *pack) {
	objfree(&net_packs_data, pack->data);

	objfree(&net_packs, pack);

	return 0;
}

PNET_PACK(PNET_PACK_TYPE_SINGLE,pnet_pack_alloc,pnet_pack_free);
