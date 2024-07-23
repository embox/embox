/**
 * @file
 *
 * @brief
 *
 * @date 26.12.2018
 * @author Alexander Kalmuk
 */
#include <stdlib.h>

#include <hal/ipl.h>
#include <kernel/task.h>
#include <embox/test.h>

#include "heap_helpers.h"

#define MIN_OBJ_SIZE OPTION_GET(NUMBER, min_obj_size)
#define MAX_OBJ_SIZE OPTION_GET(NUMBER, max_obj_size)

EMBOX_TEST_SUITE("heap extended test");

static void *task_thr_realloc_hnd(void *opaque) {
	int j;
	void *ptr, *ptr_new;
	size_t size;

	for (size = MIN_OBJ_SIZE; size < MAX_OBJ_SIZE / 2; size *= 2) {
		ptr = malloc(size);
		test_assert_not_null(ptr);

		/* Fill memory with 0x55 and last element is 0x1f */
		memset(ptr, 0x55, size - 1);
		*((char *) ptr + size - 1) = 0x1f;

#if __GNUC__ >= 12
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuse-after-free"
#endif /* __GNUC__ */
		/* Realloc object of twice size of the previously allocated one */
		ptr_new = realloc(ptr, 2 * size);
		test_assert_not_null(ptr);
#if __GNUC__ >= 12
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

		for (j = 0; j < size - 1; j++) {
			test_assert_equal(*((char *) ptr_new + j), 0x55);
		}
		test_assert_equal(*((char *) ptr_new + size - 1), 0x1f);

		free(ptr_new);
	}

	return NULL;
}

TEST_CASE("Realloc different sizes") {
	pid_t pid;

	pid = new_task("heap_extended_test_task0", task_thr_realloc_hnd, NULL);
	test_assert_true(pid >= 0);

	task_waitpid(pid);
}

static void test_max_objs_allocation(bool is_kernel_heap) {
	int count;
	size_t size;
	struct dlist_head objs_list;

	dlist_init(&objs_list);

	for (size = MIN_OBJ_SIZE; size <= MAX_OBJ_SIZE; size *= 2) {
		count = 0;
		while (1) {
			if (!test_malloc_obj(size, &objs_list, is_kernel_heap)) {
				break;
			}
			count++;
		}

		test_assert_not_zero(count);
		test_free_all_allocated_objs(&objs_list, is_kernel_heap);

		/* Check we can allocate the same count of objects as before */
		while (count--) {
			test_assert_not_null(test_malloc_obj(size, &objs_list,
				is_kernel_heap));
		}
		test_free_all_allocated_objs(&objs_list, is_kernel_heap);
	}
}

static void *task_thr_hnd(void *opaque) {
	test_max_objs_allocation(false);
	return NULL;
}

TEST_CASE("Allocate all available heap of a new task, then free all"
			" and allocate once more") {
	pid_t pid;

	pid = new_task("heap_extended_test_task", task_thr_hnd, NULL);
	test_assert_true(pid >= 0);

	task_waitpid(pid);
}

TEST_CASE("Allocate all available heap of kernel task, then free all"
			" and allocate once more") {
	ipl_t ipl;

	/* To make sure nobody uses heap during the test */
	ipl = ipl_save();

	test_max_objs_allocation(true);

	ipl_restore(ipl);
}
