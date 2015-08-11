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
#include <util/binalign.h>
#include <util/indexator.h>

static int ind_check(struct indexator *ind, size_t idx) {

	assert(ind);

	if (idx == INDEX_NONE) {
		return -EINVAL;
	}
	
	if (!((idx >= ind->start) && (idx <= ind->end))) {
		return -EINVAL;
	}

	return 0;
}

static int ind_get_bit(struct indexator *ind, size_t idx) {
	size_t word, bit;

	assert(ind != NULL);
	assert(idx != INDEX_NONE);
	assert((idx >= ind->start) && (idx <= ind->end));

	word = (idx - ind->start) / LONG_BIT;
	bit = (idx - ind->start) % LONG_BIT;

	assert(ind->mask != NULL);
	return ind->mask[word] & (1 << bit);
}

static void ind_set_bit(struct indexator *ind, size_t idx) {
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

static void ind_unset_bit(struct indexator *ind, size_t idx) {
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

static size_t ind_find_rand(struct indexator *ind) {
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

static size_t ind_find_less(struct indexator *ind, size_t idx,
		size_t min, size_t none) {
	while (idx-- > min) {
		if (!ind_get_bit(ind, idx)) {
			return idx;
		}
	}

	return none;
}

static size_t ind_find_more(struct indexator *ind, size_t idx,
		size_t max, size_t none) {
	while (idx++ < max) {
		if (!ind_get_bit(ind, idx)) {
			return idx;
		}
	}

	return none;
}

void index_init(struct indexator *ind, size_t start,
		size_t capacity, void *data) {
	assert(ind != NULL);
	assert(capacity != 0);
	assert(data != NULL);
	assert(start + capacity != INDEX_NONE);

	ind->last = INDEX_NONE;
	ind->min = ind->next = ind->start = ind->clamp_min = start;
	ind->max = ind->prev = ind->end = ind->clamp_max = start + capacity - 1;

	ind->mask = (unsigned long *)data;
	memset(data, 0, binalign_bound(capacity, CHAR_BIT) / CHAR_BIT);
}

void index_clamp(struct indexator *ind, size_t min, size_t max) {
	assert(ind != NULL);
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

void index_clean(struct indexator *ind) {
	assert(ind != NULL);
	assert(ind->mask != NULL);

	ind->last = INDEX_NONE;
	ind->min = ind->next = ind->clamp_min = ind->start;
	ind->max = ind->prev = ind->clamp_max = ind->end;

	memset(ind->mask, 0,
			binalign_bound(index_capacity(ind), CHAR_BIT) / CHAR_BIT);
}

size_t index_start(struct indexator *ind) {
	assert(ind != NULL);
	return ind->start;
}

size_t index_end(struct indexator *ind) {
	assert(ind != NULL);
	return ind->end;
}

size_t index_capacity(struct indexator *ind) {
	assert(ind != NULL);
	return ind->end - ind->start + 1;
}

size_t index_clamp_min(struct indexator *ind) {
	assert(ind != NULL);
	return ind->clamp_min;
}

size_t index_clamp_max(struct indexator *ind) {
	assert(ind != NULL);
	return ind->clamp_max;
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

	assertf(0, "unknown type");
	return INDEX_NONE;
}

int index_try_lock(struct indexator *ind, size_t idx) {
	int err;

	if ((err = ind_check(ind, idx))) {
		return err;
	}

	if (ind_get_bit(ind, idx)) {
		return 0;
	}

	index_lock(ind, idx);
	return 1;
}

void index_lock(struct indexator *ind, size_t idx) {
	ind_set_bit(ind, idx);

	assert(ind != NULL);
	assert(idx != INDEX_NONE);

	if ((idx >= ind->clamp_min) && (idx <= ind->clamp_max)) {
		ind->last = idx;

		if (idx == ind->min) {
			ind->min = ind_find_more(ind, ind->min, ind->clamp_max,
					INDEX_NONE);
		}
		if (idx == ind->max) {
			ind->max = ind_find_less(ind, ind->max, ind->clamp_min,
					INDEX_NONE);
		}
		ind->prev = ind_find_less(ind, idx, ind->clamp_min,
				ind->max);
		ind->next = ind_find_more(ind, idx, ind->clamp_max,
				ind->min);
	}
}

int index_locked(struct indexator *ind, size_t idx) {
	int err;

	if ((err = ind_check(ind, idx))) {
		return err;
	}

	return ind_get_bit(ind, idx);
}

void index_unlock(struct indexator *ind, size_t idx) {

	if (ind_check(ind, idx)) {
		return ;
	}

	ind_unset_bit(ind, idx);

	assert(ind != NULL);
	assert(idx != INDEX_NONE);

	if ((idx >= ind->clamp_min) && (idx <= ind->clamp_max)) {
		assert(ind->last != INDEX_NONE);

		if (ind->min == INDEX_NONE) {
			ind->min = ind->max = ind->prev = ind->next = idx;
		}
		else {
			if (idx < ind->min) {
				ind->min = idx;
			}
			if (idx > ind->max) {
				ind->max = idx;
			}
			if (((idx > ind->prev) && (idx < ind->last))
					|| ((ind->prev > ind->last)
						&& ((idx > ind->prev) || (idx < ind->last)))
					|| (ind->prev == ind->last)) {
				ind->prev = idx;
			}
			if (((idx < ind->next) && (idx > ind->last))
					|| ((ind->next < ind->last)
						&& ((idx < ind->next) || (idx > ind->last)))
					|| (ind->next == ind->last)) {
				ind->next = idx;
			}
		}
	}
}

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
