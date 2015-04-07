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
#include <util/err.h>
#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee.h>
#include <kernel/sched/current.h>
#include <kernel/sched/schedee_priority.h>
#include <mem/misc/pool.h>

/** locks: sched. lthread->run must be atomic. */
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

	/* TODO: SMP barrier?
	 * After finishing lt has to restore waiting state in case it is not
	 * sceduled. */
	if (!next->ready)
		next->waiting = true;

	return NULL;
}

void lthread_init(struct lthread *lt, int (*run)(struct lthread *)) {
	schedee_init(&lt->schedee, SCHED_PRIORITY_HIGH, lthread_process);
	sched_wait_info_init(&lt->info);

	lt->run = run;
	lt->label_offset = 0;
}

/** Locks: IPL, thread. */
static int __lthread_disable(struct lthread *lt) {
	assert(lt);

 	/* If lt is scheduled, have to wait till finished. */
 	if (lt->schedee.ready)
 		return false;

 	/* If lt is waiting, have to prevent waking up. */
	if (lt->info.status == SCHED_WAIT_STARTED)
		timer_close(lt->info.tmr);

	return true;
}

void lthread_reset(struct lthread *lt) {
	assert(lt);

 	while (!SPIN_IPL_PROTECTED_DO(&lt->schedee.lock, __lthread_disable(lt))) {
		schedule();
	}

	/* Leads to initial state.
 	 * TODO: reset timing? */
	sched_wait_info_init(&lt->info);
	lt->schedee.waiting = true;
	lt->label_offset = 0;
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup(&lt->schedee);
}
