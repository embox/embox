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

	assert(ind);
	assert(idx != INDEX_NONE);

	if ((idx >= ind->clamp_min) && (idx <= ind->clamp_max)) {
		ind->last = idx;

		if (idx == ind->min) {
			ind->min = ind_find_more(ind, ind->min, ind->clamp_max, INDEX_NONE);
		}
		if (idx == ind->max) {
			ind->max = ind_find_less(ind, ind->max, ind->clamp_min, INDEX_NONE);
		}
		ind->prev = ind_find_less(ind, idx, ind->clamp_min, ind->max);
		ind->next = ind_find_more(ind, idx, ind->clamp_max, ind->min);
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
		return;
	}

	ind_unset_bit(ind, idx);

	assert(ind);
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
