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
#include <kernel/lthread/lthread_priority.h>
#include <mem/misc/pool.h>

/* locks: sched. lthread->run must be atomic. */
static struct schedee *lthread_process(struct schedee *prev,
		struct schedee *next) {
	struct lthread *lt = mcast_out(next, struct lthread, schedee);
	schedee_set_current(next);

	/* lthread is not in runq, it can be waken up again. */
	next->ready = false;
	next->waiting = true;

	/* We have to restore ipl as soon as possible. */
	ipl_enable();

	lt->run_ret = next->run(next->run_arg);

	/* After finishing lt has to restore waiting state in case it is not sceduled*/
	spin_protected_if(&next->lock, !next->ready)
		next->waiting = true;

	return NULL;
}

void lthread_init(struct lthread *lt, void *(*run)(void *), void *arg) {
	schedee_init(&lt->schedee, LTHREAD_PRIORITY_DEFAULT, lthread_process, run, arg);
	sched_wait_info_init(&lt->info);
	lt->start_label = NULL;
}

static int __lthread_delete(struct lthread *lt) {
	assert(lt);

 	/* lt scheduled, have to wait till finished */
 	if (lt->schedee.ready)
 		return false;

 	/* lt is sleeping, have to prevent waking up */
	if (lt->info.status == SCHED_WAIT_STARTED)
		timer_close(lt->info.tmr);

	lt->schedee.waiting = false;

	return true;
}

void lthread_delete(struct lthread *lt) {
	assert(lt);

 	while (!SPIN_IPL_PROTECTED_DO(&lt->schedee.lock, __lthread_delete(lt))) {
		schedule();
	}
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup(&lt->schedee);
}
