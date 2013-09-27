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

enum index_type {
	INDEX_RANDOM,
	INDEX_MIN,
	INDEX_MAX,
	INDEX_PREV,
	INDEX_NEXT
};

struct indexator {
	size_t last;

	size_t min;
	size_t max;
	size_t prev;
	size_t next;

	unsigned long *mask;
	size_t start;
	size_t end;
	size_t clamp_min;
	size_t clamp_max;
};

extern void index_init(struct indexator *ind, size_t start,
		size_t capacity, void *data);
extern void index_clean(struct indexator *ind);
extern size_t index_start(struct indexator *ind);
extern size_t index_capacity(struct indexator *ind);
extern void index_clamp(struct indexator *ind, size_t min,
		size_t max);
extern size_t index_find(struct indexator *ind,
		enum index_type type);
extern int index_try_lock(struct indexator *ind, size_t idx);
extern void index_lock(struct indexator *ind, size_t idx);
extern int index_locked(struct indexator *ind, size_t idx);
extern void index_unlock(struct indexator *ind, size_t idx);
extern size_t index_alloc(struct indexator *ind,
		enum index_type type);
extern void index_free(struct indexator *ind, size_t idx);

#define INDEX_NONE (size_t)-1

#define INDEX_DATA_DEF(name, capacity) \
	static unsigned long name[(capacity + LONG_BIT - 1) / LONG_BIT]

#define INDEX_INIT(start_, capacity, data) \
	{                                      \
		.last = INDEX_NONE,                \
		.min = start_,                     \
		.max = start_ + capacity - 1,      \
		.prev = INDEX_NONE,                \
		.next = start_,                    \
		.mask = (unsigned long *)data,     \
		.start = start_,                   \
		.end = start_ + capacity - 1,      \
		.clamp_min = start_,               \
		.clamp_max = start_ + capacity - 1 \
	}

#define INDEX_DEF(name, start, capacity)             \
	INDEX_DATA_DEF(name##_data, capacity);           \
	static struct indexator name = INDEX_INIT(start, \
			capacity, &name##_data[0])

#endif /* UTIL_INDEXATOR_H_ */
