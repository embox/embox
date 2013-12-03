/**
 * @file
 *
 * @author Eldar Abusalimov
 */

#include <assert.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/time/timer.h>

void sched_wait_prepare(void) {
	struct thread *t = thread_self();

	// TODO SMP barrier? -- Eldar
	t->state &= ~THREAD_READY;
}

void sched_wait_cleanup(void) {
	struct thread *t = thread_self();

	t->state |= THREAD_READY;
	// TODO SMP barrier? -- Eldar
}

int sched_wait(void) {
	schedule();
	return 0;  // XXX
}

static void sched_wait_timeout_handler(struct sys_timer *timer, void *data) {
	struct thread *t = data;
	sched_wakeup(t);
}

int sched_wait_timeout(clock_t timeout) {
	struct sys_timer tmr;
	clock_t cur_time;
	int diff;

	if (timeout == SCHED_TIMEOUT_INFINITE)
		return sched_wait();

	cur_time = clock();
	timer_init(&tmr, TIMER_ONESHOT, timeout,
			sched_wait_timeout_handler, thread_self());
	schedule();
	diff = clock() - cur_time;
	timer_close(&tmr);

	return (diff < timeout) ? timeout - diff : 0;  // XXX
}

