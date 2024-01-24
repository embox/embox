/**
 * @file
 *
 * @brief
 *
 * @date 26.12.2018
 * @author Alexander Kalmuk
 */

#ifndef TESTS_MEM_HEAP_HELPERS_H_
#define TESTS_MEM_HEAP_HELPERS_H_

#include <stddef.h>
#include <stdbool.h>
#include <lib/libds/dlist.h>

struct test_allocated_obj {
	struct dlist_head list_link;
};

extern void *test_malloc_obj(size_t size, struct dlist_head *objs_list,
	bool is_kernel_heap);
extern void test_free_obj(void *obj, bool is_kernel_heap);
extern void test_free_all_allocated_objs(struct dlist_head *objs_list,
	bool is_kernel_heap);

#endif /* TESTS_MEM_HEAP_HELPERS_H_ */
