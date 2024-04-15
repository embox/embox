/**
 * @file
 * @brief
 *
 * @date 09.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <util/binalign.h>

#include "index_priv.h"

void index_clean(struct indexator *ind) {
	size_t buf_sz;

	assert(ind);
	assert(ind->mask);

	ind->last = INDEX_NONE;
	ind->min = ind->next = ind->clamp_min = ind->start;
	ind->max = ind->prev = ind->clamp_max = ind->end;

	buf_sz = binalign_bound(index_capacity(ind), sizeof(*ind->mask) * CHAR_BIT)
	         / CHAR_BIT;
	memset(ind->mask, 0, buf_sz);
}
