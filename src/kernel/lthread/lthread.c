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
#include <err.h>
#include <kernel/sched.h>
#include <kernel/lthread/lthread.h>
#include <kernel/sched/schedee.h>
#include <kernel/sched/current.h>
#include <kernel/lthread/lthread_priority.h>
#include <mem/misc/pool.h>

/**
 * Memory have to be allocated only for lthread structure, while lthread uses
 * thread stack when executed
 */
#define LTHREAD_POOL_SIZE OPTION_GET(NUMBER, lthread_pool_size)

POOL_DEF(lthread_pool, struct lthread, LTHREAD_POOL_SIZE);

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

static void lthread_init(struct lthread *lt, void *(*run)(void *), void *arg) {
	assert(lt);

	schedee_init(&lt->schedee, LTHREAD_PRIORITY_DEFAULT, lthread_process, run, arg);
	sched_wait_info_init(&lt->info);
}

struct lthread *lthread_create(void *(*run)(void *), void *arg) {
	struct lthread *lt;

	if (!run) {
		return err_ptr(EINVAL);
	}

	if (!(lt = (struct lthread *) pool_alloc(&lthread_pool))) {
		return err_ptr(ENOMEM);
	}

	lthread_init(lt, run, arg);

	return lt;
}

//TODO disable logic
void lthread_delete(struct lthread *lt) {
	assert(lt);

check:
	spin_protected_if(&lt->schedee.lock, lt->schedee.waiting) {
		/* cannot be launched anymore */
		lt->schedee.waiting = false;
	} else {
		schedule();
		goto check;
	}

	pool_free(&lthread_pool, lt);
}

void lthread_launch(struct lthread *lt) {
	assert(lt);
	sched_wakeup(&lt->schedee);
}
