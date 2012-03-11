/**
 * @file
 *
 * @brief
 *
 * @date 02.03.2012
 * @author Anton Bondarev
 */

#ifndef PNET_PACK_H_
#define PNET_PACK_H_

#include <util/array.h>

struct pnet_pack_desc {
	uint32_t type;
	struct pnet_pack * (*create)(void *, size_t size);
	void(*destroy)(struct pnet_pack *pack);
};

enum PNET_PACK_TYPE {
	PNET_PACK_TYPE_SKB = 0,
	PNET_PACK_TYPE_SINGLE = 1
};

#define PNET_PACK(ptype,fcreate,fdestroy) \
	extern const struct pnet_pack_desc __pnet_pack_registry[]; \
	ARRAY_SPREAD_ADD(__pnet_pack_registry, {ptype,fcreate,fdestroy});


#define pnet_pack_destroy(pack) \
		__pnet_pack_destroy(pack, pack->type)

extern struct pnet_pack *pnet_pack_create(void *buff, size_t size, uint32_t type);
extern void __pnet_pack_destroy(struct pnet_pack *pack, uint32_t type);

#endif /* PNET_PACK_H_ */
