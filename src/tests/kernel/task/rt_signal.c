/**
 * @file
 *
 * @brief Test for real-time signals
 *
 * @date 25.10.2012
 * @author Alexander Kalmuk
 */

#include <embox/test.h>

#include <signal.h>
#include <unistd.h>
#include <kernel/task.h>
#include <sys/wait.h>

EMBOX_TEST_SUITE("test for real-time signals");

#define MY_SIGRT (SIGRTMIN + 1)

static void test_sig_handler(int sig, siginfo_t *info, void *ctx) {
	test_emit('0' + (char) info->si_value.sival_int);
}

static void *test_task_entry(void *data) {
	struct sigaction act  = { 0 };
	int err;

	act.sa_flags = (SA_SIGINFO);
	act.sa_sigaction = test_sig_handler;

	err = sigaction(MY_SIGRT, &act, NULL);
	test_assert_zero(err);

	while(1)
		sleep(0);

	return NULL;
}

static void test_send_sigval(int tid, int val) {
	union sigval value;
	int err;

	value.sival_int = val;
	err = sigqueue(tid, MY_SIGRT, value);
	test_assert_zero(err);
}

TEST_CASE("send 3 rt signals of the same type between two tasks and check"
		" of handling count and order") {
	int tid;

	tid = new_task("", test_task_entry, NULL);
	test_assert(tid >= 0);

	sleep(1);

	/* Send three real-time signals with the same type. */

	test_send_sigval(tid, 1);
	test_send_sigval(tid, 2);
	test_send_sigval(tid, 3);

	sleep(1);

	/* Check order, in witch signals were handled. */
	test_assert_emitted("123");

	kill(tid, 9);
	waitpid(tid, NULL, 0);
}
