/**
 * @file
 *
 * @brief pnet's packet factory implementation
 *
 * @date 02.03.2012
 * @author Anton Bondarev
 */

#include <lib/libds/array.h>
#include <mem/objalloc.h>

#include <pnet/pack/pnet_pack.h>
#include <pnet/core/types.h>

ARRAY_SPREAD_DEF(const struct pnet_pack_desc, __pnet_pack_registry);

static const struct pnet_pack_desc *find_pack_desc(uint32_t type) {
	const struct pnet_pack_desc *ptr;
	array_spread_foreach_ptr(ptr, __pnet_pack_registry) {
		if(ptr->type == type) {
			return ptr;
		}
	}

	return NULL;
}

struct pnet_pack *pnet_pack_create(void *buff, size_t size, uint32_t type) {
	const struct pnet_pack_desc *desc;

	if(NULL == (desc = find_pack_desc(type))) {
		return NULL;
	}

	return desc->create(buff, size);
}

void pnet_pack_destroy(struct pnet_pack *pack) {
	const struct pnet_pack_desc *desc;

	desc = find_pack_desc(pack->type);

	desc->destroy(pack);
}
