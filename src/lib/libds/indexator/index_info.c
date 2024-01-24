/**
 * @file
 * @brief
 *
 * @date 09.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>

#include "index_priv.h"

size_t index_start(struct indexator *ind) {
	assert(ind);

	return ind->start;
}

size_t index_end(struct indexator *ind) {
	assert(ind);

	return ind->end;
}

size_t index_capacity(struct indexator *ind) {
	assert(ind);

	return ind->end - ind->start + 1;
}

size_t index_clamp_min(struct indexator *ind) {
	assert(ind);

	return ind->clamp_min;
}

size_t index_clamp_max(struct indexator *ind) {
	assert(ind);

	return ind->clamp_max;
}
