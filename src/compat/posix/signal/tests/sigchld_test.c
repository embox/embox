/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.04.2014
 */

#include <kernel/task.h>
#include <embox/test.h>
#include <unistd.h>

EMBOX_TEST_SUITE("sigchild test suite");

static void run_wait_task(void *(*fn)(void *), void *arg) {
	pid_t p;

	p = new_task("", fn, NULL);

	while (-EINTR == task_waitpid(p));
}

static void *sigchld_test_child(void *arg) {
	test_emit('a');
	_exit(0);
}

static void *sigchld_test_without_handler(void *arg) {
	run_wait_task(sigchld_test_child, NULL);
	test_emit('b');
	return NULL;
}

TEST_CASE("no signal handler should be triggered by default") {
	run_wait_task(sigchld_test_without_handler, NULL);
	test_assert_emitted("ab");
}

void sigchld_handler(int signal) {
	test_emit('b');
}

static void *sigchld_test_with_handler(void *arg) {
	signal(SIGCHLD, sigchld_handler);

	run_wait_task(sigchld_test_child, NULL);
	test_emit('c');
	return NULL;
}

TEST_CASE("sigchld handler should be triggered") {
	run_wait_task(sigchld_test_with_handler, NULL);
	test_assert_emitted("abc");
}


