/**
 * @file
 *
 * @date 11.12.12
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/task.h>
#include <kernel/thread/api.h>

#define KERNEL_TID      0
#define SOMEPOINTER     ((void *) 0x10000)

EMBOX_TEST_SUITE("tasks");

static void *tsk1_hnd(void *data) {
	test_assert_not_equal(task_self()->tid, KERNEL_TID);
	test_assert_equal(data, NULL);

	return NULL;
}

static void *tsk2_hnd(void *data) {
	test_assert_not_equal(task_self()->tid, KERNEL_TID);
	test_assert_equal(data, SOMEPOINTER);

	return NULL;
}

TEST_CASE("Create two tasks") {
	new_task(tsk1_hnd, NULL);
	new_task(tsk2_hnd, SOMEPOINTER);

	// TODO: waitpid
}



static void *tsk3_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);
	return NULL;
}

static void *tsk3_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_self()->tid, KERNEL_TID);
	test_assert_equal(data, NULL);
	test_assert_zero(
				thread_create(&t, 0, tsk3_thr2_hnd, task_self()));
	test_assert_equal(task_self(), t->task);

	return NULL;
}

TEST_CASE("Create task with 2 threads and terminate "
		"task in main one not detaching another") {
	new_task(tsk3_thr1_hnd, NULL);
	// TODO: waitpid
}




static void *tsk4_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);
	return NULL;
}

static void *tsk4_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_self()->tid, KERNEL_TID);
	test_assert_equal(data, NULL);
	test_assert_zero(
				thread_create(&t, 0, tsk4_thr2_hnd, task_self()));
	test_assert_equal(task_self(), t->task);
	thread_join(t, NULL);

	return NULL;
}

TEST_CASE("Create task with 2 threads and terminate "
		"task in main one with detaching another") {
	new_task(tsk4_thr1_hnd, NULL);
	// TODO: waitpid
}


static void *tsk5_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);
	task_exit(NULL);

	return NULL;
}

static void *tsk5_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_self()->tid, KERNEL_TID);
	test_assert_equal(data, NULL);
	test_assert_zero(
				thread_create(&t, 0, tsk5_thr2_hnd, task_self()));
	test_assert_equal(task_self(), t->task);
	thread_join(t, NULL);

	return NULL;
}

TEST_CASE("Create task with 2 threads and terminate "
		"task not in main one") {
	new_task(tsk5_thr1_hnd, NULL);
	// TODO: waitpid
}
