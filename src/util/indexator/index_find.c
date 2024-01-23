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

size_t index_find(struct indexator *ind, enum index_type type) {
	assert(ind);

	switch (type) {
	case INDEX_RANDOM:
		return ind_find_rand(ind);
	case INDEX_MIN:
		return ind->min;
	case INDEX_MAX:
		return ind->max;
	case INDEX_PREV:
		return ind->prev;
	case INDEX_NEXT:
		return ind->next;
	}

	assertf(0, "unknown type");

	return INDEX_NONE;
}
