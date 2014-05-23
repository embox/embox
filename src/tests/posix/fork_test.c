/**
 * @file
 *
 * @date Jan 9, 2014
 * @author: Anton Bondarev
 */

#include <sys/types.h> /* pid_t */
#include <sys/wait.h>  /* waitpid */
#include <stdio.h>     /* printf, perror */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* _exit, fork */

#include <embox/test.h>

EMBOX_TEST_SUITE("fork() testsuite");

TEST_CASE("") {
	pid_t pid;
	pid_t parent_pid;

	parent_pid = getpid();

	pid = fork();

	/* When fork() returns -1, an error happened. */
	test_assert(pid != -1);

	if (pid == 0) {
		/* When fork() returns 0, we are in the child process. */
		pid_t child_pid;

		child_pid = getpid();
		test_assert(child_pid != parent_pid);

		sleep(1);

		exit(0);
	}

	if (pid > 0) {
		/*
		 * When fork() returns a positive number, we are in the parent process
		 * and the return value is the PID of the newly created child process.
		 */
		int status;

		test_assert(pid != parent_pid);

		test_assert(getpid() == parent_pid);

		waitpid(pid, &status, 0);

		test_assert(status == 0); /* return by exit */
	}
}

pid_t fork_proxy(void) {
	pid_t pid;
	unsigned char buf[64];

	memset(buf, 0xc3, sizeof(buf));

	pid = fork();
	test_assert(pid != -1);
	if (pid > 0) {
		int i;
		for (i = 0; i < sizeof(buf); i++) {
			test_assert_equal(buf[i], 0xc3);
		}
	}

	return pid;
}

TEST_CASE("fork'ed child is allowed to crash parent stack") {
	pid_t pid;
	int res;

	pid = fork_proxy();
	if (pid == 0) {
		unsigned char buf[64];
		memset(buf, 0xa5, sizeof(buf));
		_exit(0);
	}

	wait(&res);
}
