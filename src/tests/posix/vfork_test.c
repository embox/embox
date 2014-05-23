/**
 * @file
 *
 * @data May 7, 2014
 * @author Anton Bondarev
 */

#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* wait */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* _exit, vfork */

#include <embox/test.h>

EMBOX_TEST_SUITE("vfork() testsuite");

TEST_CASE("after called vfork() child call exit()") {
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
}

#if 0
/* isn't works with kfork */
TEST_CASE("parent should see stack modifications made from child") {
	pid_t pid;
	int res;
	int data;

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
}
#endif

TEST_CASE("after called vfork() child trashes own stack and calls exit") {
	pid_t pid;
	int res;

	pid = vfork();
	if (pid == 0) {
		unsigned char buf[64];
		memset(buf, 0xa5, sizeof(buf));
		_exit(0);
	}
	wait(&res);
}

TEST_CASE("after called vfork() child call execv()") {
	pid_t pid;
	pid_t parent_pid;
	int res;

	parent_pid = getpid();

	pid = vfork();
	/* When vfork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
		/* When vfork() returns 0, we are in the child process. */
		if (execv("help", NULL) == -1) {
			test_assert(0);
		}
	}

	wait(&res);

	test_assert_not_equal(pid, parent_pid);
	test_assert_equal(getpid(), parent_pid);
}

pid_t vfork_proxy(void) {
	pid_t pid;
	unsigned char buf[64];

	memset(buf, 0xc3, sizeof(buf));

	pid = vfork();
	test_assert(pid != -1);
	if (pid > 0) {
		int i;
		for (i = 0; i < sizeof(buf); i++) {
			test_assert_equal(buf[i], 0xc3);
		}
	}

	return pid;
}

TEST_CASE("vfork'ed child is allowed to crash parent stack") {
	pid_t pid;
	int res;

	pid = vfork_proxy();
	if (pid == 0) {
		unsigned char buf[64];
		memset(buf, 0xa5, sizeof(buf));
		_exit(0);
	}

	wait(&res);
}
