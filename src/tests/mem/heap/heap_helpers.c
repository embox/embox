/**
 * @file
 *
 * @brief
 *
 * @date 26.12.2018
 * @author Alexander Kalmuk
 */
#include <stdlib.h>
#include <mem/sysmalloc.h>
#include "heap_helpers.h"

void *test_malloc_obj(size_t size, struct dlist_head *objs_list,
		bool is_kernel_heap) {
	struct test_allocated_obj *obj;

	obj = is_kernel_heap ? sysmalloc(size) : malloc(size);
	if (!obj) {
		return NULL;
	}
	dlist_head_init(&obj->list_link);
	dlist_add_prev(&obj->list_link, objs_list);

	return obj;
}

void test_free_obj(void *opaque, bool is_kernel_heap) {
	struct test_allocated_obj *obj = opaque;

	dlist_del(&obj->list_link);
	is_kernel_heap ? sysfree(obj) : free(obj);
}

void test_free_all_allocated_objs(struct dlist_head *objs_list,
		bool is_kernel_heap) {
	struct test_allocated_obj *obj;

	dlist_foreach_entry(obj, objs_list, list_link) {
		test_free_obj(obj, is_kernel_heap);
	}
}
