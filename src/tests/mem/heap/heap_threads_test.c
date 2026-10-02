/**
 * @file
 *
 * @brief
 *
 * @date 26.12.2018
 * @author Alexander Kalmuk
 */
#include <kernel/task.h>
#include <kernel/thread.h>
#include <util/err.h>
#include <embox/test.h>

#include "heap_helpers.h"

EMBOX_TEST_SUITE("heap threads tests");

#define TEST_OBJ_SIZE OPTION_GET(NUMBER, test_obj_size)

/* Maximum number of objects of size TEST_OBJ_SIZE that can be allocated 
 * within the task heap */
static size_t max_objs_count;

struct test_thr_heap_info {
	struct dlist_head objs_list;
	size_t count;
};

/* malloc() while some space left */
static size_t thread_malloc_objs(struct dlist_head *objs_list) {
	size_t count = 0;

	while (1) {
		if (!test_malloc_obj(TEST_OBJ_SIZE, objs_list, false)) {
			break;
		}
		count++;
	}

	return count;
}

static void *task_thr2_hnd(void *data) {
	struct test_thr_heap_info *thr2_info = data;

	thr2_info->count = thread_malloc_objs(&thr2_info->objs_list);
	test_assert_not_zero(thr2_info->count);

	return NULL;
}

static void *task_thr1_hnd(void *data) {
	struct test_thr_heap_info *thr_info_array = data;
	struct test_thr_heap_info *thr1_info = &thr_info_array[0];
	struct thread *t;

	/* First, evalutes the maximum number of objects to malloc() from heap */
	max_objs_count = thread_malloc_objs(&thr1_info->objs_list);
	test_free_all_allocated_objs(&thr1_info->objs_list, false);

	/* Now create te second thread and call malloc() simultaneously */
	t = thread_create(0, task_thr2_hnd, &thr_info_array[1]);
	test_assert_zero(ptr2err(t));

	thr1_info->count = thread_malloc_objs(&thr1_info->objs_list);
	test_assert_not_zero(thr1_info->count);

	return NULL;
}

TEST_CASE("Two threads call malloc() simultaneously multiple times") {
	pid_t pid;
	struct test_thr_heap_info thr_info[2];

	dlist_init(&thr_info[0].objs_list);
	dlist_init(&thr_info[1].objs_list);

	pid = new_task("", task_thr1_hnd, thr_info);
	test_assert_true(pid >= 0);

	task_waitpid(pid);

	test_free_all_allocated_objs(&thr_info[0].objs_list, false);
	test_free_all_allocated_objs(&thr_info[1].objs_list, false);

	/* Maximum number of allocated object must be equal to the sum
	 * of numbers of allocated objects per thread */
	test_assert_equal(max_objs_count, thr_info[0].count + thr_info[1].count);
}
