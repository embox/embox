/**
 * @file
 * @brief
 *
 * @date 7.08.2014
 * @author Vita Loginova
 */

#include <assert.h>
#include <errno.h>

#include <kernel/sched/sync/mutex.h>
#include <kernel/thread/waitq.h>
#include <kernel/sched/schedee_priority.h>


void mutex_init_schedee(struct mutex *m) {
	waitq_init(&m->wq);
	m->lock_count = 0;
	m->holder = NULL;

	mutexattr_init(&m->attr);
}

int mutex_trylock_schedee(struct schedee *self, struct mutex *m) {
	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (m->holder) {
		return -EBUSY;
	}

	m->lock_count = 1;
	m->holder = self;

	return 0;
}

void mutex_unlock_schedee(struct schedee *self, struct mutex *m) {
	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	mutex_priority_uninherit(self);

	m->holder = NULL;
	m->lock_count = 0;
	waitq_wakeup_all(&m->wq);
}

/* The fallback below is for the build with no inheritance at all, where
 * schedee_priority_inherit() answers 0 and the set is the only thing that
 * happens. Written as `!=' it fired in a second case nobody meant: when the
 * holder is ALREADY higher than the waiter, inherit correctly changes nothing
 * and answers the holder's own priority, which differs from the waiter's --
 * and the set then wrote the waiter's priority into the holder's BASE. A
 * low-priority thread taking a lock for a moment permanently demoted whoever
 * held it.
 *
 * `<' says what was meant: fall back only when the boost did not take. With
 * inheritance built in it never does; with `none' it always does, exactly as
 * before. */
void mutex_priority_inherit(struct schedee *self, struct mutex *m) {
	int prior = schedee_priority_get(self);

	if (schedee_priority_inherit(m->holder, prior) < prior) {
		schedee_priority_set(m->holder, prior);
	}
}

void mutex_priority_uninherit(struct schedee *self) {
	/* Giving the boost back is schedee_priority_reverse()'s whole job. What
	 * was here compared schedee_priority_get(self) with
	 * schedee_priority_reverse(self) -- two calls whose order of evaluation C
	 * does not define, one of which changes what the other returns. Read the
	 * wrong way the fallback wrote the BOOSTED priority into the base, making
	 * a temporary boost permanent. The fallback was for the `none' build,
	 * where schedee_priority_set() is a no-op anyway. */
	schedee_priority_reverse(self);
}

