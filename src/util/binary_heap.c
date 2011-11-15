/**
 * @file
 * @brief Implementation of heap
 *
 * @date 17.10.11
 * @author Gleb Efimov
 */

#include <util/binary_heap.h>
#include <util/list.h>

struct list_link abs_min; /* This is my "absolute" minimum */

struct bh *bh_init(struct bh *binary_heap) {
	binary_heap->counter = -1;
	return binary_heap;
}

int bh_add(struct bh *binary_heap, struct list_link *value) {
	binary_heap->counter++;
	list_add_last_link(&abs_min, binary_heap->elements);
	return sort_bh(binary_heap, value);
}

int sort_bh(struct bh *binary_heap, struct list_link *value) {
	return 0;
}
