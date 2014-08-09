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
#include <unistd.h>    /* exit, fork */

#include <kernel/task.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("fork() testsuite");

#define fork_test_assert(_expr) \
	do { \
		if (!(_expr)) { \
			exit(1); \
		} \
	} while (0)

static void *test_fork_basic(void *arg) {
	pid_t pid;
	pid_t parent_pid;
	int cnt = 5;

	parent_pid = getpid();

	pid = fork();

	/* When fork() returns -1, an error happened. */
	fork_test_assert(pid != -1);

	if (pid == 0) {
		/* When fork() returns 0, we are in the child process. */
		pid_t child_pid;
		int cnt = 5;

		child_pid = getpid();
		fork_test_assert(child_pid != parent_pid);

		while (cnt-- > 0) {
			sleep(0);
		}

		exit(0);
	}

	while(cnt-- > 0) {
		sleep(0);
	}

	if (pid > 0) {
		/*
		 * When fork() returns a positive number, we are in the parent process
		 * and the return value is the PID of the newly created child process.
		 */
		int status;

		fork_test_assert(pid != parent_pid);

		fork_test_assert(getpid() == parent_pid);

		waitpid(pid, &status, 0);

		fork_test_assert(status == 0); /* return by exit */
	}

	exit(0);
}

TEST_CASE("fork should pass basic sanity checks") {
	int res;

	/* creating new task such way to have it's heap clean */
	new_task("", test_fork_basic, NULL);

	wait(&res);

	test_assert_zero(res);
}

static pid_t fork_proxy(void) {
	pid_t pid;
	unsigned char buf[64];

	memset(buf, 0xc3, sizeof(buf));

	pid = fork();
	fork_test_assert(pid != -1);
	if (pid > 0) {
		int i;
		for (i = 0; i < sizeof(buf); i++) {
			fork_test_assert(buf[i] == 0xc3);
		}
	}

	return pid;
}

static void *test_fork_stack_crash(void *arg) {
	pid_t pid;
	int res;

	pid = fork_proxy();
	if (pid == 0) {
		unsigned char buf[64];
		memset(buf, 0xa5, sizeof(buf));
		exit(0);
	}

	wait(&res);

	exit(0);
}

TEST_CASE("fork'ed child is allowed to crash parent stack") {
	int res;

	/* creating new task such way to have it's heap clean */
	new_task("", test_fork_stack_crash, NULL);

	wait(&res);

	test_assert_zero(res);
}

#define TEST_FORK_MALLOC_SZ 128
static void *test_fork_heap_crash(void *arg) {
	pid_t pid;
	int i, res;

	unsigned char *ptr = malloc(TEST_FORK_MALLOC_SZ);
	if (!ptr) {
		exit(1);
	}

	memset(ptr, 0xc3, TEST_FORK_MALLOC_SZ);

	pid = fork();
	if (pid == 0) {
		memset(ptr, 0xa5, TEST_FORK_MALLOC_SZ);
		free(ptr);
		exit(0);
	}

	wait(&res);

	for (i = 0; i < TEST_FORK_MALLOC_SZ; i++) {
		if (ptr[i] != 0xc3) {
			exit(2);
		}
	}

	free(ptr);

	exit(0);
}

TEST_CASE("fork'ed child is allowed to crash heap") {
	int res;

	/* creating new task such way to have it's heap clean */
	new_task("", test_fork_heap_crash, NULL);

	wait(&res);

	test_assert_zero(res);
}

static void *test_fork_static_crash(void *arg) {

	execv("test_fork_static", NULL);

	fprintf(stderr, "%s: can't exec", __func__);
	abort();
}

TEST_CASE("fork'ed child is allowed to crash static memory") {
	int res;

	/* creating new task such way to have it's heap clean */
	new_task("", test_fork_static_crash, NULL);

	wait(&res);

	test_assert_zero(res);
}

