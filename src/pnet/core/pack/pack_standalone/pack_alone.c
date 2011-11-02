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

#define NET_PACKS_CNT 0x10
#define PACK_DATA_LEN 64

OBJALLOC_DEF(net_packs, struct net_packet, NET_PACKS_CNT);
OBJALLOC_DEF(net_packs_data, unsigned char[PACK_DATA_LEN], NET_PACKS_CNT);

net_packet_t pnet_pack_alloc(net_node_t node, enum net_packet_dir dir, void *data, int len) {
	net_packet_t pack = objalloc(&net_packs);

	pack->node = node;
	pack->dir  = dir;

	pack->data = objalloc(&net_packs_data);

	memcpy(pack->data, data, len);

	pack->len = len;

	return pack;
}

int pnet_pack_free(net_packet_t pack) {
	objfree(&net_packs_data, pack->data);

	objfree(&net_packs, pack);

	return 0;
}
