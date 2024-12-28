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

void index_init(struct indexator *ind, size_t start, size_t capacity,
    void *data) {
	assert(ind);
	assert(data);
	assert(capacity);
	assert(start + capacity != INDEX_NONE);

	ind->last = INDEX_NONE;
	ind->min = ind->next = ind->start = ind->clamp_min = start;
	ind->max = ind->prev = ind->end = ind->clamp_max = start + capacity - 1;

	ind->mask = (unsigned long *)data;
	memset(data, 0, (INDEX_DATA_LEN(capacity) * INDEX_DATA_BIT) / CHAR_BIT);
}
