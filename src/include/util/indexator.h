/*
 * @file
 *
 * @date Oct 10, 2012
 * @author: Anton Bondarev
 */

#ifndef INDEXATOR_H_
#define INDEXATOR_H_

#include <types.h>

enum indexator_allocation_type {
	INDEX_ALLOC_MIN = 0,
	INDEX_ALLOC_RANDOM = 1,
	INDEX_ALLOC_MAX = 2
};


struct indexator {
	int min_busy;
	int max_busy;

	uint32_t *masks;
};

extern struct indexator *indexator_init(struct indexator *indexator, uint32_t *mask_array, size_t len_mask);

extern int index_find(struct indexator * indexator, enum indexator_allocation_type allocation_type);

extern void index_lock(struct indexator * indexator, int idx);

extern int index_alloc(struct indexator * indexator, enum indexator_allocation_type allocation_type);

extern int index_locked(struct indexator * indexator, int idx);

extern void index_unlock(struct indexator * indexator, int idx);

extern void index_free(struct indexator * indexator, int idx);


#endif /* INDEXATOR_H_ */
