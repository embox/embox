/**
 * @file
 * @brief pnet skb type packet definitions
 *
 * @date 02.11.2011
 * @author Anton Kozlov
 */

#ifndef PNET_PACK_SKB_H_
#define PNET_PACK_SKB_H_

#include <net/skbuff.h>
#include <pnet/pnet_pack.h>

static inline void *pnet_pack_get_data(struct pnet_pack *pack) {
	struct sk_buff *skb = pack->data;
	return skb->mac.raw;
}

static inline int pnet_pack_get_len(struct pnet_pack *pack) {
	struct sk_buff *skb = pack->data;
	return skb->len;
}

#endif
