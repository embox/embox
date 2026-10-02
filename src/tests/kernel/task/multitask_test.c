/**
 * @file
 *
 * @date 11.12.12
 * @author Anton Bulychev
 */

#include <sys/types.h>
#include <unistd.h>

#include <util/err.h>
#include <embox/test.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>

#define KERNEL_TID      (task_get_id(task_kernel_task()))
#define SOMEPOINTER     ((void *) 0x10000)
#define EXIT_CODE1       0x03
#define EXIT_CODE2       0x99

EMBOX_TEST_SUITE("task/multi tests");

static void *tsk1_hnd(void *data) {
	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	return (void *)EXIT_CODE1;
}

static void *tsk2_hnd(void *data) {
	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, SOMEPOINTER);

	return (void *)EXIT_CODE2;
}

TEST_CASE("Create two tasks") {
	pid_t pid1, pid2;

	pid1 = new_task("", tsk1_hnd, NULL);
	test_assert_true(pid1 >= 0);

	pid2 = new_task("", tsk2_hnd, SOMEPOINTER);
	test_assert_true(pid2 >= 0);

	test_assert_zero(sleep(1));

	test_assert_equal(EXIT_CODE1, task_waitpid(pid1));
	test_assert_equal(EXIT_CODE2, task_waitpid(pid2));
}

static void *tsk3_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);
	return (void *)EXIT_CODE2;
}

static void *tsk3_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	t = thread_create(0, tsk3_thr2_hnd, task_self());
	test_assert_zero(ptr2err(t));

	return (void *)EXIT_CODE1;
}

TEST_CASE("Create task with 2 threads and terminate "
		"task in main one not detaching another") {
	pid_t pid;

	pid = new_task("", tsk3_thr1_hnd, NULL);
	test_assert_true(pid >= 0);

	test_assert_equal(EXIT_CODE1, task_waitpid(pid));
}

static void *tsk4_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);
	return (void *)EXIT_CODE2;
}

static void *tsk4_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	t = thread_create(0, tsk4_thr2_hnd, task_self());
	test_assert_zero(ptr2err(t));

	test_assert_zero(thread_join(t, NULL));

	return (void *)EXIT_CODE1;
}

TEST_CASE("Create task with 2 threads and terminate "
		"task in main one with detaching another") {
	pid_t pid;

	pid = new_task("", tsk4_thr1_hnd, NULL);
	test_assert_true(pid >= 0);

	test_assert_equal(EXIT_CODE1, task_waitpid(pid));
}

static void *tsk5_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);
	task_exit((void *)EXIT_CODE2);

	return (void *)(~EXIT_CODE2);
}

static void *tsk5_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	t = thread_create(0, tsk5_thr2_hnd, task_self());
	test_assert_zero(ptr2err(t));

	test_assert_zero(thread_join(t, NULL));

	return (void *)EXIT_CODE1;
}

TEST_CASE("Create task with 2 threads and terminate "
		"task not in main one") {
	pid_t pid;

	pid = new_task("", tsk5_thr1_hnd, NULL);
	test_assert_true(pid >= 0);

	test_assert_equal(EXIT_CODE2, task_waitpid(pid));
}

static void *tsk6_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);

	return (void *)EXIT_CODE2;
}

static void *tsk6_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	t = thread_create(0, tsk6_thr2_hnd, task_self());
	test_assert_zero(ptr2err(t));

	test_assert_zero(thread_join(t, NULL));

	thread_exit((void *)EXIT_CODE1);

	return (void *)(~EXIT_CODE1);
}

TEST_CASE("Create task with 2 threads and terminate "
		"task in main thread by exiting threads") {
	pid_t pid;

	pid = new_task("", tsk6_thr1_hnd, NULL);
	test_assert_true(pid >= 0);

	test_assert_equal(EXIT_CODE1, task_waitpid(pid));
}

static void *tsk7_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);

	return (void *)EXIT_CODE2;
}

static void *tsk7_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	t = thread_create(0, tsk7_thr2_hnd, task_self());
	test_assert_zero(ptr2err(t));

	thread_exit((void *)EXIT_CODE1);

	return (void *)(~EXIT_CODE1);
}

TEST_CASE("Create task with 2 threads and terminate "
		"task by exiting threads") {
	pid_t pid;

	pid = new_task("", tsk7_thr1_hnd, NULL);
	test_assert_true(pid >= 0);

	test_assert_equal(EXIT_CODE1, task_waitpid(pid));
}

static void *tsk8_thr2_hnd(void *data) {
	test_assert_equal(task_self(), data);

	return (void *)EXIT_CODE2;
}

static void *tsk8_thr1_hnd(void *data) {
	struct thread *t;

	test_assert_not_equal(task_get_id(task_self()), KERNEL_TID);
	test_assert_equal(data, NULL);

	t = thread_create(0, tsk8_thr2_hnd, task_self());
	test_assert_zero(ptr2err(t));

	test_assert_zero(thread_join(t, NULL));

	task_exit((void *)EXIT_CODE1);

	return (void *)(~EXIT_CODE1);
}

TEST_CASE("Create task with 2 threads and terminate "
		"task in main one by task_exit()") {
	pid_t pid;

	pid = new_task("", tsk8_thr1_hnd, NULL);
	test_assert_true(pid >= 0);

	test_assert_equal(EXIT_CODE1, task_waitpid(pid));
}
