/**
 * @file
 * @brief
 *
 * @author  Andrey Kokorev
 * @author  Vita Loginova
 * @date    21.11.2013
 */

#include <stdbool.h>
#include <assert.h>
#include <errno.h>

#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee_priority.h>

#include <kernel/time/timer.h>

/** locks: IPL, sched. lthread->run must be atomic. */
static struct schedee *lthread_process(struct schedee *prev,
		struct schedee *next) {
	struct lthread *lt = mcast_out(next, struct lthread, schedee);
	schedee_set_current(next);

	/* lthread is not in runq, it can be waken up again. */
	next->ready = false;
	next->waiting = true;

	/* We have to restore ipl as soon as possible. */
	ipl_enable();

	lt->label_offset = lt->run(lt);

	return NULL;
}

void lthread_init(struct lthread *lt, int (*run)(struct lthread *)) {
	schedee_init(&lt->schedee, SCHED_PRIORITY_HIGH, lthread_process);
	sched_wait_info_init(&lt->info);

	lt->run = run;
	lt->label_offset = 0;
}

/** Locks: IPL, thread. */
int __lthread_is_disabled(struct lthread *lt) {
	assert(lt);
	return lt->schedee.waiting && lt->info.status == 0;
}

int lthread_reset(struct lthread *lt) {
	assert(lt);

	if (!SPIN_IPL_PROTECTED_DO(&lt->schedee.lock, __lthread_is_disabled(lt)))
		return -EAGAIN;

	/* Leads to initial state.
	 * TODO: reset timing? */
	sched_wait_info_init(&lt->info);
	lt->schedee.waiting = true;
	lt->label_offset = 0;

	return 0;
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup(&lt->schedee);
}
