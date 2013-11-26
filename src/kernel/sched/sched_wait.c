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
	sched_switch();
	return 0;  // XXX
}

static void waitq_timeout_handler(struct sys_timer *timer, void *data) {
	struct thread *t = data;
	sched_wakeup(t); // -ETIMEDOUT
}

int sched_wait_timeout(int timeout) {
	struct sys_timer tmr;
	int ret;

	if (timeout == SCHED_TIMEOUT_INFINITE)
		return sched_wait();

	ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t) timeout,
			waitq_timeout_handler, thread_self());
	if (ret)
		return ret;

	sched_switch();

	timer_close(&tmr);  // TODO err check?

	return 0;  // XXX
}

