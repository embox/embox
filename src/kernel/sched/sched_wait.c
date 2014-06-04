/**
 * @file
 *
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stdbool.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/time/timer.h>

void sched_wait_prepare(void) {
	struct thread *t = thread_self();

	// TODO SMP barrier? -- Eldar
	t->waiting = true;
}

void sched_wait_cleanup(void) {
	struct thread *t = thread_self();

	t->waiting = false;
	// TODO SMP barrier? -- Eldar
}

static int sched_intr(int res) {
	struct thread *t = thread_self();
	struct sigstate *sigst = &t->sigstate;
	int sig;
	siginfo_t sinfo;

	/*if (t->sigaction) {*/
	if ((sig = sigstate_receive(sigst, &sinfo))) {
		sigstate_send(sigst, sig, &sinfo);
		return -EINTR;
	}

	return res;
}

int sched_wait(void) {
	schedule();
	return sched_intr(0);
}

static void sched_wait_timeout_handler(struct sys_timer *timer, void *data) {
	struct thread *t = data;
	sched_wakeup(t);
}

int sched_wait_timeout(clock_t timeout, clock_t *remain) {
	struct sys_timer *tmr;
	clock_t remain_v, cur_time;
	int res, diff;

	if (timeout == SCHED_TIMEOUT_INFINITE) {
		remain_v = SCHED_TIMEOUT_INFINITE;
		res = sched_wait();
		goto out;
	}

	cur_time = clock();
	if ((res = timer_set(&tmr, TIMER_ONESHOT, jiffies2ms(timeout),
			sched_wait_timeout_handler, thread_self()))) {
		return res;
	}

	schedule();
	diff = clock() - cur_time;
	timer_close(tmr);

	if (diff < timeout) {
		remain_v = timeout - diff;
		res = 0;
	} else {
		remain_v = 0;
		res = -ETIMEDOUT;
	}

	res = sched_intr(res);
out:
	if (remain) {
		*remain = remain_v;
	}
	return res;
}

