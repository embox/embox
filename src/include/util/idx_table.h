/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.06.2012
 */

#ifndef UTIL_IDX_TABLE_H_
#define UTIL_IDX_TABLE_H_


#include <assert.h>
#include <util/num_alloc.h>
#include <stddef.h>

typedef struct util_idx_table_struct {
	int n;
	util_num_alloc_t *num_alloc;
	void *table[];
} util_idx_table_t;

#define UTIL_IDX_TABLE_DEF_INLINE(_type, _name, _n) \
	static struct { \
		util_idx_table_t idx_table; \
		_type __idx_table_storage[_n]; \
	} _name##_tuple; \
	static util_idx_table_t *_name = &_name##_tuple.idx_table

#define UTIL_IDX_TABLE_DEF(_type, _name, _n) \
	UTIL_NUM_ALLOC_DEF(_name##_num_alloc, _n) ;\
	UTIL_IDX_TABLE_DEF_INLINE(_type, _name, _n)

static inline void util_idx_table_init(util_idx_table_t *idx_table, int n,
		util_num_alloc_t *num_alloc) {
	idx_table->n = n;
	idx_table->num_alloc = num_alloc;

	for (int i = 0; i < n; i++) {
		idx_table->table[i] = NULL;
	}
}

#define UTIL_IDX_TABLE_INIT(_name, _n) \
	util_num_alloc_init((util_num_alloc_t *) &_name##_num_alloc, _n); \
	util_idx_table_init((util_idx_table_t *) _name, _n, \
		       (util_num_alloc_t *) &_name##_num_alloc)

#define UTIL_IDX_TABLE_CALC(_type, _n) \
	(sizeof(util_idx_table_t) + sizeof(_type) * _n)

static inline int util_idx_table_add(util_idx_table_t *idx_table, void *data) {
	int n = util_num_alloc(idx_table->num_alloc);
	if (n < 0) {
		return n;
	}

	idx_table->table[n] = data;

	return n;
}

static inline void *util_idx_table_get(util_idx_table_t *idx_table, int n) {
	assert((n >= 0) && (n < idx_table->n));
	return idx_table->table[n];
}

static inline void util_idx_table_del(util_idx_table_t *idx_table, int n) {
	util_num_free(idx_table->num_alloc, n);

	idx_table->table[n] = NULL;
}

static inline void util_idx_table_set(util_idx_table_t *idx_table, int n, void *data) {
	if (util_idx_table_get(idx_table, n)) {
		util_idx_table_del(idx_table, n);
	}

	if (data) {
		assert(util_num_alloc_specified(idx_table->num_alloc, n) >= 0);

		idx_table->table[n] = data;
	}
}

static inline int util_idx_table_next_alloc(util_idx_table_t *idx_table) {
	return util_num_alloc_next_alloc(idx_table->num_alloc);
}

static inline int util_idx_table_next_mark(util_idx_table_t *idx_table, int i, char mark) {
	return util_num_alloc_next_mark(idx_table->num_alloc, i, mark);
}

#endif /* UTIL_IDX_TABLE_H_ */
