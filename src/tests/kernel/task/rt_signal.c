/**
 * @file
 *
 * @brief Test for real-time signals
 *
 * @date 25.10.2012
 * @author Alexander Kalmuk
 */

#include <embox/test.h>
#include <kernel/task.h>
#include <kernel/task/rt_signal.h>
#include <signal.h>
#include <unistd.h>

EMBOX_TEST_SUITE("test for real-time signals");

#define MY_SIGRT (SIGRTMIN + 1)

static int handled_sig_cnt;

static void sig_handler(int sig, union sigval value) {
	test_emit('0' + (char)value.sival_int);
	handled_sig_cnt++;
}

static void *task_hnd(void *data) {
	handled_sig_cnt=0;
	task_self_rtsignal_set(MY_SIGRT, sig_handler);
	while(1);
	return NULL;
}

TEST_CASE("send 3 rt signals of the same type between two tasks and check"
		" of handling count and order") {
	union sigval value;
	int tid = new_task(task_hnd, NULL);

	test_assert(tid >= 0);

	sleep(0);

	/* Send three real-time signals with the same type. */

	value.sival_int = 1;
	sigqueue(tid, MY_SIGRT, value);

	value.sival_int = 2;
	sigqueue(tid, MY_SIGRT, value);

	value.sival_int = 3;
	sigqueue(tid, MY_SIGRT, value);

	/* Wait until all signals will be handled. */
	while (handled_sig_cnt != 3) {}

	/* Check order, in witch signals were handled. */
	test_assert_emitted("123");

	kill(tid, 9);
}
