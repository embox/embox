/**
 * @file
 *
 * @date Oct 9, 2012
 * @author: Anton Bondarev
 */

#include <util/indexator.h>
#include <string.h>

struct indexator *indexator_init(struct indexator *indexator, uint32_t start_idx, uint32_t *mask_array, size_t len_mask) {

	indexator->masks = mask_array;
	indexator->min_busy = start_idx - 1;
	indexator->max_busy = len_mask * sizeof(mask_array);
	indexator->capacity = len_mask * sizeof(mask_array);
	indexator->start = start_idx;

	memset(mask_array, 0, len_mask * 32);

	return NULL;
}

int index_locked(struct indexator * indexator, int idx) {
	int word_offset;
	int bit_offset;

	word_offset = (idx - indexator->start) / 32;
	bit_offset = (idx - indexator->start) % 32;

	return (indexator->masks[word_offset] & (1 << bit_offset)) ? 1 : 0;
}

int index_find(struct indexator * indexator, enum indexator_allocation_type allocation_type) {
	int cur_idx;
	int word_offset;
	int bit_offset;

	cur_idx = indexator->min_busy + 1;
	word_offset = cur_idx / 32;
	bit_offset = cur_idx % 32;

	while(cur_idx < indexator->max_busy) {
		if(0xFFFFFFFF == indexator->masks[word_offset]) {
			cur_idx += (32 - bit_offset);
			bit_offset = 0;
			continue;
		}
		while(bit_offset < 32) {
			if(0 == (indexator->masks[word_offset] & 1 << bit_offset)) {
				return indexator->start + cur_idx;
			}
			bit_offset++;
			cur_idx++;
		}
		bit_offset = 0;
	}

	return -1;
}

void index_lock(struct indexator * indexator, int idx) {
	int word_offset;
	int bit_offset;

	word_offset = (idx - indexator->start) / 32;
	bit_offset = (idx - indexator->start) % 32;

	indexator->masks[word_offset] |= (1 << bit_offset);
}

void index_unlock(struct indexator * indexator, int idx) {
	int word_offset;
	int bit_offset;

	word_offset = (idx - indexator->start) / 32;
	bit_offset = (idx - indexator->start) % 32;

	indexator->masks[word_offset] &= ~(1 << bit_offset);
	if(idx < indexator->min_busy) {
		return;
	}
	if(idx > indexator->max_busy) {
		return;
	}
}

int index_alloc(struct indexator * indexator, enum indexator_allocation_type allocation_type) {
	int idx;
	if(-1 == (idx = index_find(indexator, allocation_type))) {
		return -1;
	}
	index_lock(indexator, idx);
	return idx;
}

void index_free(struct indexator * indexator, int idx) {
	index_unlock(indexator, idx);
}
