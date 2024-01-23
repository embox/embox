/**
 * @file
 * @brief
 *
 * @date 09.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <stddef.h>

#include "index_priv.h"

size_t index_alloc(struct indexator *ind, enum index_type type) {
	size_t idx;

	idx = index_find(ind, type);
	if (idx != INDEX_NONE) {
		index_lock(ind, idx);
	}

	return idx;
}

void index_free(struct indexator *ind, size_t idx) {
	index_unlock(ind, idx);
}
