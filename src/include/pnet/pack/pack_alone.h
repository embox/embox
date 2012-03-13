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

#include <pnet/pnet_pack.h>


static inline void *pnet_pack_get_data(struct pnet_pack *pack) {
	struct pnet_pack_data *data = pack->data;
	return data->buff;
}

static inline int pnet_pack_get_len(struct pnet_pack *pack) {
	struct pnet_pack_data *data = pack->data;
	return data->len;
}

#endif
