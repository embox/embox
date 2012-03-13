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
#include <pnet/pnet_pack.h>

OBJALLOC_DEF(net_packs, struct pnet_pack, CONFIG_PNET_PACKETS_QUANTITY);

void pnet_pack_free(struct pnet_pack *pack) {
	kfree_skb(pack->data);

	objfree(&net_packs, pack);
}

static struct pnet_pack *pnet_pack_alloc(void *data, size_t len) {
	struct pnet_pack *pack;

	if(NULL == (pack = objalloc(&net_packs))) {
		return NULL;
	}

	if(NULL == data) {
		if(NULL == (pack->data = alloc_skb(len, 0))) {
			pnet_pack_free(pack);
			return NULL;
		}
	} else {
		pack->data = data;
	}

	pack->dir = PNET_PACK_DIRECTION_RX;

	pack->type = PNET_PACK_TYPE_SKB;

	return pack;
}

PNET_PACK(PNET_PACK_TYPE_SKB,pnet_pack_alloc,pnet_pack_free);
