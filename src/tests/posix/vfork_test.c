/**
 * @file
 *
 * @data May 7, 2014
 * @author Anton Bondarev
 */

#include <sys/types.h> /* pid_t */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* _exit, vfork */

#include <embox/test.h>

EMBOX_TEST_SUITE("vfork() testsuite");

TEST_CASE("after called vfork() child call exit()") {
	pid_t pid;
	pid_t parent_pid;

	parent_pid = getpid();

	pid = vfork();
	/* When vfork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
		/* When vfork() returns 0, we are in the child process. */
		_exit(0);
	}

	test_assert_not_equal(pid, parent_pid);
	test_assert_equal(getpid(), parent_pid);
}

TEST_CASE("after called vfork() child call execve()") {
	pid_t pid;
	pid_t parent_pid;

	parent_pid = getpid();

	pid = vfork();
	/* When vfork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
#if 0
		//TODO execve() isn't implemented yet
		/* When vfork() returns 0, we are in the child process. */
		if (execve("help", NULL, NULL) == -1) {
			test_assert(0);
		}
#endif
		_exit(0);
	}

	test_assert_not_equal(pid, parent_pid);
	test_assert_equal(getpid(), parent_pid);
}
