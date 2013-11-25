/**
 * @file
 *
 * @author Eldar Abusalimov
 */

#include <assert.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>
#include <kernel/thread.h>
#include <kernel/time/timer.h>

int sched_wait(void) {
	struct thread *current = thread_self();
	sched_switch();
	return current->waitq_link.result;
}

static void waitq_timeout_handler(struct sys_timer *timer, void *data) {
	struct thread *t = data;
	sched_wakeup(t, -ETIMEDOUT);
}

int sched_wait_timeout(int timeout) {
	struct thread *current = thread_self();
	struct sys_timer tmr;
	int ret;

	if (timeout == SCHED_TIMEOUT_INFINITE)
		return sched_wait();

	ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t) timeout,
			waitq_timeout_handler, current);
	if (ret)
		return ret;

	sched_switch();

	timer_close(&tmr);  // TODO err check?

	return current->waitq_link.result;
}

/** Called with IRQs off and thread lock held. */
void __sched_wait_prepare(struct waitq *wq, int result) {
	struct thread *t = thread_self();

	t->waitq_link.result = result;

	assert(!t->waitq);
	t->waitq = wq;
	if (wq)
		waitq_enqueue(wq, t);

	t->state &= ~THREAD_READY;
}

void sched_wait_prepare(struct waitq *wq, int result) {
	struct thread *t = thread_self();
	ipl_t ipl;

	/* Disabling just IRQs could enough because waiting is performed on the
	 * current thread. */
	ipl = spin_lock_ipl(&t->lock);
	__sched_wait_prepare(wq, result);
	spin_unlock_ipl(&t->lock, ipl);
}

/** Called with IRQs off and thread lock held. */
int __sched_wait_cleanup(struct waitq *wq) {
	struct thread *t = thread_self();
	int result;

	t->state |= THREAD_READY;

	if (wq && t->waitq == wq) {
		waitq_remove(wq, t);
		t->waitq = NULL;
	}

	result = t->waitq_link.result;

	return result;
}

int sched_wait_cleanup(struct waitq *wq) {
	struct thread *t = thread_self();
	int result;
	ipl_t ipl;

	ipl = spin_lock_ipl(&t->lock);
	result = __sched_wait_cleanup(wq);
	spin_unlock_ipl(&t->lock, ipl);

	return result;
}

/** Called with IRQs off and thread lock held. */
int __sched_wakeup(struct thread *t, int result) {
	assert(t);

	if (t->state & THREAD_READY)
		return 0;

	t->waitq_link.result = result;

	if (t->waitq) {
		waitq_remove(t->waitq, t);
		t->waitq = NULL;
	}
	__sched_start(t);

	return 1;
}

int sched_wakeup(struct thread *t, int result) {
	int ret;
	ipl_t ipl;

	assert(t);

	ipl = spin_lock_ipl(&t->lock);
	ret = __sched_wakeup(t, result);
	spin_unlock_ipl(&t->lock, ipl);

	return ret;
}

void sched_wakeup_waitq(struct waitq *wq, int nr, int result) {
	struct thread *t;
	ipl_t ipl;

	assert(wq);

	while ((t = waitq_dequeue(wq))) {

		ipl = spin_lock_ipl(&t->lock);
		/* Someone could wake up a thread before locking directly and
		 * it could have a time to go asleep again on _another_ waitq. */
		if (t->waitq == wq) {
			t->waitq = NULL;
			if (__sched_wakeup(t, result))
				--nr;
		}
		spin_unlock_ipl(&t->lock, ipl);

		if (!nr)
			break;
	}
}
