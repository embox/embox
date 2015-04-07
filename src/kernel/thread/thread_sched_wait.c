/**
 * @file
 *
 * @author Eldar Abusalimov
 */

#include <assert.h>
#include <stdbool.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/time/timer.h>

#include <kernel/sched/current.h>

#include <kernel/thread.h>

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
	struct schedee *s = data;
	sched_wakeup(s);
}

#if OPTION_GET(BOOLEAN, timer_allocate_on_stack)
#define SCHED_WAIT_TIMER_DEF(_tmr) \
	struct sys_timer _tmr;

#define SCHED_WAIT_TIMER_INIT(_tmr, ...) \
	timer_init(&_tmr, __VA_ARGS__)

#define SCHED_WAIT_TIMER_CLOSE(_tmr) \
	timer_close(&_tmr)
#else
#define SCHED_WAIT_TIMER_DEF(_tmr) \
	struct sys_timer *_tmr;

#define SCHED_WAIT_TIMER_INIT(_tmr, ...) \
	timer_set(&_tmr, __VA_ARGS__)

#define SCHED_WAIT_TIMER_CLOSE(_tmr) \
	timer_close(_tmr)
#endif

int sched_wait_timeout(clock_t timeout, clock_t *remain) {
	SCHED_WAIT_TIMER_DEF(tmr);
	clock_t remain_v, cur_time;
	int res, diff;

	if (timeout == SCHED_TIMEOUT_INFINITE) {
		remain_v = SCHED_TIMEOUT_INFINITE;
		res = sched_wait();
		goto out;
	}

	cur_time = clock();
	if ((res = SCHED_WAIT_TIMER_INIT(tmr, TIMER_ONESHOT, jiffies2ms(timeout),
			sched_wait_timeout_handler, schedee_get_current()))) {
		return res;
	}

	schedule();
	diff = clock() - cur_time;
	SCHED_WAIT_TIMER_CLOSE(tmr);

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
