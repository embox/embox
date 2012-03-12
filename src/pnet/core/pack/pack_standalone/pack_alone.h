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

struct pnet_pack {
	struct pnet_pack_data *data;
	enum PNET_PACK_DIRECTION dir;
	struct net_node *node;
	uint32_t type;
};

static inline void *pnet_pack_get_data(struct pnet_pack *pack) {
	return pack->data->buff;
}

static inline int pnet_pack_get_len(struct pnet_pack *pack) {
	return pack->data->len;
}

#endif
