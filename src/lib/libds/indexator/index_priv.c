/**
 * @file
 * @brief
 *
 * @date 09.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>

#include <lib/libds/indexator.h>

int ind_check(struct indexator *ind, size_t idx) {
	assert(ind);

	if (idx == INDEX_NONE) {
		return -EINVAL;
	}

	if (!((idx >= ind->start) && (idx <= ind->end))) {
		return -EINVAL;
	}

	return 0;
}

int ind_get_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	assert(ind->mask != NULL);

	return ind->mask[word] & (1 << bit);
}

void ind_set_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	assert(ind->mask != NULL);
	assert(~ind->mask[word] & (1 << bit));

	ind->mask[word] |= (1 << bit);
}

void ind_unset_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	assert(ind->mask != NULL);
	assert(ind->mask[word] & (1 << bit));

	ind->mask[word] &= ~(1 << bit);
}

size_t ind_find_rand(struct indexator *ind) {
	size_t idx, capacity;

	assert(ind != NULL);

	if (ind->min == INDEX_NONE) {
		return INDEX_NONE;
	}

	capacity = ind->clamp_max - ind->clamp_min + 1;
	do {
		idx = ind->clamp_min + rand() % capacity;
	} while (ind_get_bit(ind, idx));

	return idx;
}

size_t ind_find_less(struct indexator *ind, size_t idx, size_t min,
    size_t none) {
	while (idx-- > min) {
		if (!ind_get_bit(ind, idx)) {
			return idx;
		}
	}

	return none;
}

size_t ind_find_more(struct indexator *ind, size_t idx, size_t max,
    size_t none) {
	while (idx++ < max) {
		if (!ind_get_bit(ind, idx)) {
			return idx;
		}
	}

	return none;
}
