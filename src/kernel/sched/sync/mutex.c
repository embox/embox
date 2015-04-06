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
#include <kernel/sched/sched_priority.h>


void mutex_init_schedee(struct mutex *m) {
	waitq_init(&m->wq);
	m->lock_count = 0;
	m->holder = NULL;

	mutexattr_init(&m->attr);
}

int mutex_trylock_schedee(struct mutex *m) {
	struct schedee *current = schedee_get_current();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	if (m->holder) {
		return -EBUSY;
	}

	m->lock_count = 1;
	m->holder = current;

	return 0;
}

void mutex_unlock_schedee(struct mutex *m) {
	struct schedee *current = schedee_get_current();

	assert(m);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	priority_uninherit(current);

	m->holder = NULL;
	m->lock_count = 0;
	waitq_wakeup_all(&m->wq);
}

void priority_inherit(struct schedee *s, struct mutex *m) {
	sched_priority_t prior = schedee_priority_get(s);

	if (prior != schedee_priority_inherit(m->holder, prior))
		schedee_priority_set(m->holder, prior);
}

void priority_uninherit(struct schedee *s) {
	sched_priority_t prior = schedee_priority_get(s);

	if (prior != schedee_priority_reverse(s))
		schedee_priority_set(s, schedee_priority_get(s));
}

