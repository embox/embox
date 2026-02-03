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

#include <kernel/thread/thread_wait.h>

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


void thread_wait_init(struct thread_wait *tw) {
	dlist_init(&tw->thread_waitq_list);
}

static inline void thread_wait_add(struct thread_wait *tw, struct sys_timer *tmr) {
	dlist_add_next(&tw->thread_waitq_list, dlist_init(&tmr->st_wait_link));
}

static inline void thread_wait_del(struct sys_timer *tmr) {
	dlist_del(&tmr->st_wait_link);
}

void thread_wait_deinit(struct thread_wait *tw) {
	struct sys_timer *tmr = NULL;

	dlist_foreach_entry_safe(tmr, &tw->thread_waitq_list, st_wait_link) {
		sys_timer_close(tmr);
	}
	dlist_init(&tw->thread_waitq_list);
}

int sched_wait_timeout(clock_t timeout, clock_t *remain) {
	struct sys_timer tmr;
	clock_t remain_v, cur_time;
	int res, diff;
	struct thread *thr;

	thr = thread_self();

	if (timeout == SCHED_TIMEOUT_INFINITE) {
		remain_v = SCHED_TIMEOUT_INFINITE;
		res = sched_wait();
		goto out;
	}

	cur_time = clock();
	thread_wait_add(&thr->thread_wait_list, &tmr);
	if ((res = sys_timer_init_start_msec(&tmr, SYS_TIMER_ONESHOT, jiffies2ms(timeout),
			sched_wait_timeout_handler, schedee_get_current()))) {
		thread_wait_del(&tmr);
		return res;
	}

	schedule();
	diff = clock() - cur_time;

	sys_timer_close(&tmr);
	thread_wait_del(&tmr);

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
