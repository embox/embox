/**
 * @file
 * @brief pnet packet for linux network layer. It encapsulates sk_buff structure
 *
 * @details This type of pnet packet make possible to use pnet with standard
 *          linux  network layer. The linux network driver allocates
 *          sk_buff structure and  calls netif_rx function. If we want  to use
 *          pnet features we must pack the sk_buff to the appropriate format
 *          pnet_pack structure. To use this types of packet you should call
 *          pnet_packet_create with the PNET_PACK_TYPE_SKB flag.
 *
 * @date 20.10.2011
 * @author Anton Kozlov
 */

#include <string.h>
#include <mem/objalloc.h>
#include <embox/unit.h>

#include <net/skbuff.h>

#include <pnet/core.h>
#include <pnet/pnet_pack.h>
#include <pnet/pack/pack_skbuf.h>

OBJALLOC_DEF(net_packs, struct pnet_pack, CONFIG_PNET_PACKETS_QUANTITY);

static void pnet_pack_free(struct pnet_pack *pack) {
	//skb_free(pack->data);

	objfree(&net_packs, pack);
}

static struct pnet_pack *pnet_pack_alloc(void *data, size_t len) {
	struct pnet_pack *pack;

	if(NULL == (pack = objalloc(&net_packs))) {
		return NULL;
	}

	if(NULL == data) {
		if(NULL == (pack->data = skb_alloc(len))) {
			pnet_pack_free(pack);
			return NULL;
		}
	} else {
		pack->data = data;
	}

	pack->dir = PNET_PACK_DIRECTION_RX;

	pack->type = PNET_PACK_TYPE_SKB;

	pack->priority = 0;

	return pack;
}

PNET_PACK(PNET_PACK_TYPE_SKB, pnet_pack_alloc, pnet_pack_free);
