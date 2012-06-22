/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    21.06.2012
 */

#ifndef UTIL_NUM_ALLOC_ARRAY_H_
#define UTIL_NUM_ALLOC_ARRAY_H_

typedef struct __util_num_alloc_struct {
	int n;
	char mark_table[];
} __util_num_alloc_t;

#define __UTIL_NUM_ALLOC_CALC(n) \
	(sizeof(__util_num_alloc_t) + n)

#define __UTIL_NUM_ALLOC_DEF_INLINE(name, _n) \
	union { \
		__util_num_alloc_t num_alloc; \
		char storage[UTIL_NUM_ALLOC_CALC(_n)];\
	} name

#define __UTIL_NUM_ALLOC_DEF(_name, _n) \
	static __UTIL_NUM_ALLOC_DEF_INLINE(_name, _n)

#endif /* UTIL_NUM_ALLOC_ARRAY_H_ */
