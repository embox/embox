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

struct net_packet {
	struct sk_buff *skbuf;

	enum net_packet_dir dir;

	net_node_t node;
};


static inline void *pnet_pack_get_data(net_packet_t pack) {
	return pack->skbuf->data;
}

static inline int pnet_pack_get_len(net_packet_t pack) {
	return pack->skbuf->len;
}

#endif
