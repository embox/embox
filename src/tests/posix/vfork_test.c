/**
 * @file
 *
 * @date May 7, 2014
 * @author Anton Bondarev
 */

#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* _exit, vfork */

#include <kernel/task.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("vfork() testsuite");

static void *test_vfork_first(void *arg) {
	pid_t pid;
	pid_t parent_pid;
	int res;

	parent_pid = getpid();

	pid = vfork();
	/* When vfork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
		/* When vfork() returns 0, we are in the child process. */
		_exit(0);
	}

	wait(&res);
	test_assert_not_equal(pid, parent_pid);
	test_assert_equal(getpid(), parent_pid);

	exit(0);
}

TEST_CASE("after called vfork() child call exit()") {
	int res;

	/* Create new task to have single thread in vfork calling task */
	new_task("", test_vfork_first, NULL);

	wait(&res);

	test_assert_zero(res);
}

static void *test_vfork_second(void *arg) {
	pid_t pid;
	int res;
	volatile int data;

	data = 1;

	pid = vfork();
	/* When vfork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
		data = 2;
		/* When vfork() returns 0, we are in the child process. */
		_exit(0);
	}

	wait(&res);
	test_assert_equal(data, 2);

	exit(0);
}

TEST_CASE("parent should see stack modifications made from child") {
	int res;

	/* Create new task to have single thread in vfork calling task */
	new_task("", test_vfork_second, NULL);

	wait(&res);

	test_assert_zero(res);
}

static void *test_vfork_third(void *arg) {
	pid_t pid;
	int res;

	pid = vfork();

	test_assert(pid != -1);

	if (pid == 0) {
		unsigned char buf[64];
		memset(buf, 0xa5, sizeof(buf));
		_exit(0);
	}
	wait(&res);

	exit(0);
}

TEST_CASE("after called vfork() child trashes own stack and calls exit") {
	int res;

	/* Create new task to have single thread in vfork calling task */
	new_task("", test_vfork_third, NULL);

	wait(&res);

	test_assert_zero(res);
}

static void *test_vfork_fourth(void *arg) {
	pid_t pid;
	pid_t parent_pid;
	int res;
	char *cmd[] = { "help", (char *)0 };

	parent_pid = getpid();

	pid = vfork();
	/* When vfork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
		close(0);
		close(1);
		close(2);
		/* When vfork() returns 0, we are in the child process. */
		if (execv("help", cmd) == -1) {
			test_assert(0);
		}
	}

	wait(&res);

	test_assert_not_equal(pid, parent_pid);
	test_assert_equal(getpid(), parent_pid);

	exit(0);
}

TEST_CASE("after called vfork() child call execv()") {
	int res;

	/* Create new task to have single thread in vfork calling task */
	new_task("", test_vfork_fourth, NULL);

	wait(&res);

	test_assert_zero(res);
}

