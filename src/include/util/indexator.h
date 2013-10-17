/*
 * @file
 * @brief
 *
 * @date 10.10.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef UTIL_INDEXATOR_H_
#define UTIL_INDEXATOR_H_

#include <stddef.h>
#include <limits.h>

/**
 * Type of index, alloced by index_alloc()
 */
enum index_type {
	INDEX_RANDOM, /* Random index */
	INDEX_MIN,    /* Minimal index */
	INDEX_MAX,    /* Maximum index */
	INDEX_PREV,   /* Previous index */
	INDEX_NEXT    /* Next index */
};

/**
 * Indexator
 */
struct indexator {
	size_t last;         /* Last allocated index */

	size_t min;          /* Minimal free index */
	size_t max;          /* Maximun free index */
	size_t prev;         /* Previous free index */
	size_t next;         /* Next free index */

	unsigned long *mask; /* Indexator storage */
	size_t start;        /* First index */
	size_t end;          /* Last index */
	size_t clamp_min;    /* Minimal possible index */
	size_t clamp_max;    /* Maximum possible index */
};

typedef unsigned long index_data_t;

#define INDEX_DATA_BIT LONG_BIT

/**
 * Indexator data length
 *
 * @param capacity - count of indexes
 * @return Number of index_data_t enough to maintain requred number of indeces.
 */
#define INDEX_DATA_LEN(capacity) \
	((capacity + INDEX_DATA_BIT - 1) / INDEX_DATA_BIT)

/**
 * Indexator data defination
 */
#define INDEX_DATA_DEF(name, capacity) \
	static index_data_t name[INDEX_DATA_LEN(capacity)]

/**
 * Indexator initialization
 */
#define INDEX_INIT(start, capacity, data)      \
	__INDEX_INIT(start, start + capacity - 1,  \
			data, start, start + capacity - 1)
/**
 * Indexator with clamp initialization
 */
#define INDEX_CLAMP_INIT(start, capacity, data, \
		clamp_min, clamp_max)                   \
	__INDEX_INIT(start, start + capacity - 1,   \
			data, clamp_min, clamp_max - 1)

/**
 * Indexator definition
 */
#define INDEX_DEF(name, start, capacity)             \
	INDEX_DATA_DEF(name##_data, capacity);           \
	static struct indexator name = INDEX_INIT(start, \
			capacity, &name##_data[0])

/**
 * Indexator with clamp definition
 */
#define INDEX_CLAMP_DEF(name, start, capacity, clamp_min,    \
		clamp_max)                                           \
	INDEX_DATA_DEF(name##_data, capacity);                   \
	static struct indexator name = INDEX_CLAMP_INIT(start,   \
			capacity, &name##_data[0], clamp_min, clamp_max)

/**
 * Index not found
 */
#define INDEX_NONE (size_t)-1

/**
 * Initialize indexator.
 *
 * @param ind - pointer to indexator
 * @param start - start of index
 * @param capacity - capacity of indexator
 * @param data - index storage
 */
extern void index_init(struct indexator *ind, size_t start,
		size_t capacity, void *data);

/**
 * Clamp indexes allocated by index_alloc().
 *
 * @param ind - pointer to indexator
 * @param min - minimal index that can be allocated
 * @param max - maximum index that can be allocated
 */
extern void index_clamp(struct indexator *ind, size_t min,
		size_t max);

/**
 * Clean up indexator and unlock the all locked indexes.
 *
 * @param ind - pointer to indexator
 */
extern void index_clean(struct indexator *ind);

/**
 * Get the first index.
 *
 * @param ind - pointer to indexator
 * @return first index of indexator
 */
extern size_t index_start(struct indexator *ind);

/**
 * Get the last index.
 *
 * @param ind - pointer to indexator
 * @return last index of indexator
 */
extern size_t index_end(struct indexator *ind);

/**
 * Get the number of indexes.
 *
 * @param ind - pointer to indexator
 * @return number of indexes of indexator
 */
extern size_t index_capacity(struct indexator *ind);

/**
 * Get the first index that can be allocated by index_alloc().
 *
 * @param ind - pointer to indexator
 * @return first possible index of indexator
 */
extern size_t index_clamp_min(struct indexator *ind);

/**
 * Get the last index that can be allocated by index_alloc().
 *
 * @param ind - pointer to indexator
 * @return last possible index of indexator
 */
extern size_t index_clamp_max(struct indexator *ind);

/**
 * Find the index from indexator according to index_type.
 *
 * @param ind - pointer to indexator
 * @param type - type of index that should be found
 * @return index according to type
 */
extern size_t index_find(struct indexator *ind,
		enum index_type type);

/**
 * Try lock the index from indexator.
 *
 * @param ind - pointer to indexator
 * @param idx - index to lock
 * @return non-zero if locked successfully otherwise zero
 */
extern int index_try_lock(struct indexator *ind, size_t idx);

/**
 * Lock the index from indexator.
 *
 * @param ind - pointer to indexator
 * @param idx - index to lock
 */
extern void index_lock(struct indexator *ind, size_t idx);

/**
 * Check the index for locking.
 *
 * @param ind - pointer to indexator
 * @param idx - index to check
 * @return non-zero if index locked otherwise zero
 */
extern int index_locked(struct indexator *ind, size_t idx);

/**
 * Unlock the index from indexator.
 *
 * @param ind - pointer to indexator
 * @param idx - index to unlock
 */
extern void index_unlock(struct indexator *ind, size_t idx);

/**
 * Alloc the index from indexator according to index_type.
 *
 * @param ind - pointer to indexator
 * @param type - type of index that should be found
 * @return index according to type
 */
extern size_t index_alloc(struct indexator *ind,
		enum index_type type);

/**
 * Free the index from indexator allocated by index_alloc().
 * This is the same as index_unlock().
 *
 * @param ind - pointer to indexator
 * @param idx - index to unlock
 */
extern void index_free(struct indexator *ind, size_t idx);


#define __INDEX_INIT(start_, end_, data, \
		clamp_min_, clamp_max_)          \
	{                                    \
		.last = INDEX_NONE,              \
		.min = clamp_min_,               \
		.max = clamp_max_,               \
		.prev = clamp_max_,              \
		.next = clamp_min_,              \
		.mask = (unsigned long *)data,   \
		.start = start_,                 \
		.end = end_,                     \
		.clamp_min = clamp_min_,         \
		.clamp_max = clamp_max_          \
	}

#endif /* UTIL_INDEXATOR_H_ */
