/**
 * @file
 *
 * @brief
 *
 * @date 26.12.2018
 * @author Alexander Kalmuk
 */
#include <kernel/printk.h>
#include <kernel/task.h>
#include <kernel/thread.h>
#include <util/err.h>
#include <embox/test.h>

#include "heap_helpers.h"

EMBOX_TEST_SUITE("heap threads tests");

#define TEST_OBJ_SIZE OPTION_GET(NUMBER, test_obj_size)

/* How many objects each thread allocates.
 *
 * This used to be "as many as fit": thread_malloc_objs() called malloc() until
 * it returned NULL, and the suite then asserted that two threads together
 * allocate exactly as many as one thread alone. Both only make sense for a
 * task heap of a FIXED size.
 *
 * On a board whose mspace grows on demand out of physical memory, malloc()
 * does not return NULL until all of it is gone, and mspace_do_alloc()
 * restarts its walk over every 1 MiB segment for every object, so the cost
 * is quadratic in the segments. The suite did not fail, it ground: the board
 * sat inside malloc() with the other three cores waiting on the BKL.
 *
 * So the count is bounded and the assertion says what the suite is actually
 * for: two threads in malloc() at the same time, both served. */
#define TEST_OBJS_MAX OPTION_GET(NUMBER, test_objs_max)

static size_t max_objs_count;

/* Weak: a kernel without the counters reads zero and the line is not printed. */
extern unsigned long mspace_alloc_steps __attribute__((weak));
extern unsigned long mspace_free_steps __attribute__((weak));

struct test_thr_heap_info {
	struct dlist_head objs_list;
	size_t count;
};

/* malloc() while some space left, up to TEST_OBJS_MAX of them */
static size_t thread_malloc_objs(struct dlist_head *objs_list) {
	size_t count = 0;

	while (count < TEST_OBJS_MAX) {
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

	/* Freed HERE, in the task that allocated. malloc() and free()
	 * work on task_self_mspace(), so a block allocated in this task cannot be
	 * freed by the test case, which runs in another one: mspace_free() fails,
	 * the fallback to the kernel task's mspace fails too, and free() ends in
	 * assert(0). The suite never reached that before because it ground to a
	 * halt in the allocation loop first. */
	test_free_all_allocated_objs(&thr2_info->objs_list, false);

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

	/* Same task, same reason as above. */
	test_free_all_allocated_objs(&thr1_info->objs_list, false);

	/* And wait for the other one. Returning from here ends the task, and
	 * task_do_exit() releases the task's resources -- its heap among them.
	 * On one core the second thread was never running at that moment; on
	 * four it is, and its next free() finds its own pointer in neither the
	 * task's mspace nor the kernel's. One run in twenty-five. A thread
	 * nobody joins is a thread the task exit races with. */
	thread_join(t, NULL);

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

	/* Nothing to free here: both lists were freed by the threads that filled
	 * them, which is the only task allowed to. */

	/* This used to assert that the two threads together allocated
	 * exactly as many objects as one thread alone had -- which is a statement
	 * about a heap that runs out, and this one grows instead.
	 *
	 * What the suite is for survives the change: both threads were inside
	 * malloc() at the same time and both were served in full. That the same
	 * address was never handed to both is checked for free by the frees
	 * above -- test_malloc_obj() links every object into a list, and a
	 * pointer returned twice would have been linked twice and taken the walk
	 * apart. */
	test_assert_equal(max_objs_count, thr_info[0].count);
	test_assert_equal(max_objs_count, thr_info[1].count);

	/* What the two linear searches over the segment list cost for this
	 * workload. Printed rather than asserted -- the number depends on how much
	 * memory the heap grew into, and the point is to make a shape visible, not
	 * to fix a value. */
	if (&mspace_alloc_steps) {
		printk("heap: %lu segment(s) visited allocating, %lu freeing, "
		       "for %lu objects\n",
		    mspace_alloc_steps, mspace_free_steps,
		    (unsigned long)(thr_info[0].count + thr_info[1].count) * 2);
	}
}
