/**
 * @file
 * @brief Network type pack defs
 *
 * @date 02.11.2011
 * @author Anton Kozlov
 */

#ifndef PNET_PACK_SKB_H_
#define PNET_PACK_SKB_H_

struct pnet_pack_data {
	void *buff;
	int len;
};

#if 0
struct net_packet {
	struct net_packet_data data;

	enum PNET_PACK_DIRECTION dir;
	net_node_t node;
};
#endif

struct pnet_pack {
	struct pnet_pack_data *data;
	enum PNET_PACK_DIRECTION dir;
	net_node_t node;
};

static inline void *pnet_pack_get_data(struct pnet_pack *pack) {
	return pack->data->buff;
}

static inline int pnet_pack_get_len(struct pnet_pack *pack) {
	return pack->data->len;
}

#endif
