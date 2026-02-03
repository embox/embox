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
#include <kernel/sched/current.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee_priority.h>

#include <kernel/time/sys_timer.h>

int __lthread_is_disabled(struct lthread *lt) {
	assert(lt);

	return SPIN_IPL_PROTECTED_DO(&lt->schedee.lock,
		lt->schedee.waiting && lt->info.status == 0);
}

/** locks: IPL, sched. lthread->run must be atomic. */
struct schedee *lthread_do_process(struct schedee *prev,
		struct schedee *next) {
	struct lthread *lt = mcast_out(next, struct lthread, schedee);

	/* lthread is not in runq, it can be waken up again. */
	next->ready = false;
	next->waiting = true;

	/* We have to enable ipl as soon as possible. */
	ipl_enable();

	lt->label_offset = lt->run(lt);

	if (lt->joining && __lthread_is_disabled(lt))
		sched_wakeup(lt->joining);

	return NULL;
}

__attribute__((weak)) struct schedee *lthread_process(
		struct schedee *prev, struct schedee *next) {
	return lthread_do_process(prev, next);
}

void lthread_init(struct lthread *lt, int (*run)(struct lthread *)) {
	assert(lt);

	schedee_init(&lt->schedee, SCHED_PRIORITY_HIGH, lthread_process,
	             SCHEDEE_LTHREAD);
	sched_wait_info_init(&lt->info);

	lt->run = run;
	lt->joining = NULL;
	lt->label_offset = 0;
}

void lthread_join(struct lthread *lt) {
	struct schedee *self = schedee_get_current();

	assert(lt);

	sched_wait_prepare();

	if (!__lthread_is_disabled(lt)) {
		assert(!lt->joining);
		lt->joining = self;
		schedule();
	}

	sched_wait_cleanup();

	/* Leads to initial state.
	 * TODO: reset timing? */
	sched_wait_info_init(&lt->info);
	lt->schedee.waiting = true;

	lt->joining = NULL;
	lt->label_offset = 0;
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup(&lt->schedee);
}
