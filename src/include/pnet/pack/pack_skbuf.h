/**
 * @file
 * @brief Network type pack defs
 *
 * @date 02.11.2011
 * @author Anton Kozlov
 */

#ifndef PNET_PACK_SKB_H_
#define PNET_PACK_SKB_H_

struct sk_buff;
#if 0
struct net_packet {
	struct sk_buff *skbuf;

	enum PNET_PACK_DIRECTION dir;

	net_node_t node;
};
#endif

struct pnet_pack {
	struct sk_buff *skb;
	uint32_t type;

	net_node_t node;
	enum PNET_PACK_DIRECTION dir;
};


static inline void *pnet_pack_get_data(struct pnet_pack *pack) {
	return pack->skb->data;
}

static inline int pnet_pack_get_len(struct pnet_pack *pack) {
	return pack->skb->len;
}

extern void pnet_pack_extract_skb(struct pnet_pack *pack);

#endif
