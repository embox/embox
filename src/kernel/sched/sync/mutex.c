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

void mutex_priority_inherit(struct schedee *self, struct mutex *m) {
	int prior = schedee_priority_get(self);

	if (prior != schedee_priority_inherit(m->holder, prior))
		schedee_priority_set(m->holder, prior);
}

void mutex_priority_uninherit(struct schedee *self) {
	if (schedee_priority_get(self) != schedee_priority_reverse(self))
		schedee_priority_set(self, schedee_priority_get(self));
}

