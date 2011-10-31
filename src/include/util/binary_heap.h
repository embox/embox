 /**
 * @file   binary_heap.h
 * @brief  Heap
 *
 * @date   17.10.2011
 * @author Gleb Efimov
 */

#ifndef UTIL_HEAP_H_
#define UTIL_HEAP_H_

#include <util/list.h>

typedef struct bh_t {
	struct list *elements;
	int counter;
}bh_t;

extern bh_t *bh_init();

int bh_count();

extern int bh_add(bh_t *heap,struct list_link *link);

extern int bh_remove(bh_t *heap, struct list_link *link);

#endif /* UTIL_HEAP_H_ */
