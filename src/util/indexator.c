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
#include <string.h>
#include <util/indexator.h>

static int ind_get_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	return ind->mask[word] & (1 << bit);
}

static void ind_set_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	assert(~ind->mask[word] & (1 << bit));
	ind->mask[word] |= (1 << bit);
}

static void ind_unset_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	assert(ind->mask[word] & (1 << bit));
	ind->mask[word] &= ~(1 << bit);
}

static size_t ind_find_rand(struct indexator *ind) {
	size_t idx, capacity;

	assert(ind != NULL);

	if (ind->min == INDEX_NONE) {
		return INDEX_NONE;
	}

	capacity = ind->clamp_max - ind->clamp_min + 1;
	do {
		idx = ind->start + rand() % capacity;
	} while (ind_get_bit(ind, idx));

	return idx;
}

static size_t ind_find_more(struct indexator *ind, size_t idx,
		size_t max) {
	while (idx++ < max) {
		if (!ind_get_bit(ind, idx)) {
			return idx;
		}
	}

	return INDEX_NONE;
}

static size_t ind_find_less(struct indexator *ind, size_t idx,
		size_t min) {
	while (idx-- > min) {
		if (!ind_get_bit(ind, idx)) {
			return idx;
		}
	}

	return INDEX_NONE;
}

void index_init(struct indexator *ind, size_t start,
		size_t capacity, void *data) {
	assert(ind != NULL);
	assert(capacity != 0);
	assert(data != NULL);
	assert(start + capacity != INDEX_NONE);

	ind->last = ind->prev = INDEX_NONE;
	ind->min = ind->next = ind->start = ind->clamp_min = start;
	ind->max = ind->end = ind->clamp_max = start + capacity - 1;

	ind->mask = (unsigned long *)data;
	memset(data, 0, (capacity + CHAR_BIT - 1) / CHAR_BIT);
}

size_t index_capacity(struct indexator *ind) {
	assert(ind != NULL);
	return ind->end - ind->start + 1;
}

void index_clamp(struct indexator *ind, size_t min, size_t max) {
	assert(ind != NULL);
	assert(min >= ind->start);
	assert(max <= ind->end);
	assert(ind->last == INDEX_NONE);

	ind->min = ind->next = ind->clamp_min = min;
	ind->max = ind->clamp_max = max;

	assert(!index_locked(ind, ind->min));
	assert(!index_locked(ind, ind->max));
	assert(!index_locked(ind, ind->next));
	assert(ind->prev == INDEX_NONE);
}

size_t index_find(struct indexator *ind, enum index_type type) {
	assert(ind != NULL);

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

	assert(0, "unknown type");
	return INDEX_NONE;
}

int index_lock(struct indexator *ind, size_t idx) {
	if (ind_get_bit(ind, idx)) {
		return -EBUSY;
	}

	ind_set_bit(ind, idx);

	ind->last = idx;
	if (idx == ind->min) {
		ind->min = ind_find_more(ind, ind->min, ind->clamp_max);
	}
	if (idx == ind->max) {
		ind->max = ind_find_less(ind, ind->max, ind->clamp_min);
	}
	ind->prev = ind_find_less(ind, idx, ind->clamp_min);
	ind->next = ind_find_more(ind, idx, ind->clamp_max);

	return 0;
}

int index_locked(struct indexator *ind, size_t idx) {
	return ind_get_bit(ind, idx);
}

void index_unlock(struct indexator *ind, size_t idx) {
	ind_unset_bit(ind, idx);

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert(ind->last != INDEX_NONE);

	if ((idx >= ind->clamp_min) && (idx < ind->min)) {
		ind->min = idx;
	}
	if ((idx > ind->max) && (idx <= ind->clamp_max)) {
		ind->max = idx;
	}
	if ((idx > ind->prev) && (idx < ind->last)) {
		ind->prev = idx;
	}
	if ((idx > ind->last) && (idx < ind->next)) {
		ind->next = idx;
	}
}

size_t index_alloc(struct indexator *ind, enum index_type type) {
	int ret;
	size_t idx;

	idx = index_find(ind, type);
	if (idx != INDEX_NONE) {
		ret = index_lock(ind, idx);
		assert(ret == 0);
	}

	return idx;
}

void index_free(struct indexator *ind, size_t idx) {
	index_unlock(ind, idx);
}
