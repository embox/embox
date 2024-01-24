/**
 * @file
 * @brief
 *
 * @date 09.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stddef.h>

#include "index_priv.h"

void index_clamp(struct indexator *ind, size_t min, size_t max) {
	assert(ind);
	assert(min >= ind->start);
	assert(max <= ind->end);
	assert(ind->last == INDEX_NONE);

	ind->min = ind->next = ind->clamp_min = min;
	ind->max = ind->prev = ind->clamp_max = max;

	assert(!index_locked(ind, ind->min));
	assert(!index_locked(ind, ind->max));
	assert(!index_locked(ind, ind->prev));
	assert(!index_locked(ind, ind->next));
}
