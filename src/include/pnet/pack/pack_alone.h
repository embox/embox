/**
 * @file
 * @brief The simple pnet packet definitions
 *
 * @date 02.11.2011
 * @author Anton Kozlov
 */

#ifndef PNET_PACK_PACK_ALONE_H_
#define PNET_PACK_PACK_ALONE_H_

#define PNET_PACK_MAX_DATA_LEN 64

struct pnet_pack_data {
	int len;
	char buff[PNET_PACK_MAX_DATA_LEN];
};

#include <pnet/pnet_pack.h>
#include <assert.h>

static inline char *pnet_pack_get_data(struct pnet_pack *pack) {
	assert(pack->type == PNET_PACK_TYPE_SINGLE);
	return (((struct pnet_pack_data *) pack->data)->buff);
}

static inline int pnet_pack_get_len(struct pnet_pack *pack) {
	assert(pack->type == PNET_PACK_TYPE_SINGLE);
	return (((struct pnet_pack_data *) pack->data)->len);
}

#endif /* PNET_PACK_PACK_ALONE_H_ */
