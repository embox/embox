 /**
 * @file
 * @brief heap data structure created using a binary tree
 *
 * @date 17.10.11
 * @author Gleb Efimov
 */

#ifndef UTIL_HEAP_H_
#define UTIL_HEAP_H_

#include <util/list.h>

struct bh {
	struct list *elements;
	int counter;
};

extern struct bh *bh_init(struct bh *binary_heap);

extern int bh_add(struct bh *heap,struct list_link *link);

extern int bh_remove(struct bh *heap, struct list_link *link);

extern int bh_count(void);

extern int sort_bh(struct bh *binary_heap, struct list_link *value);

#endif /* UTIL_HEAP_H_ */
