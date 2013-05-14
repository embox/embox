/**
 * @file
 * @brief Implements semaphore methods.
 *
 * @date 02.09.12
 * @author Anton Bulychev
 */

#include <assert.h>
#include <errno.h>
#include <kernel/thread/sync/sem.h>
#include <kernel/thread/sched.h>

static int tryenter_sched_lock(sem_t *s);

void sem_init(sem_t *s, int val) {
	wait_queue_init(&s->wq);
	s->value = 0;
	s->max_value = val;
}

void sem_enter(sem_t *s) {
	assert(s);
	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		while (tryenter_sched_lock(s) != 0) {
			wait_queue_wait_locked(&s->wq, SCHED_TIMEOUT_INFINITE);
		}
	}
	sched_unlock();
}

static int tryenter_sched_lock(sem_t *s) {
	assert(s);
	assert(critical_inside(CRITICAL_SCHED_LOCK));

	if (s->value == s->max_value) {
		return -EAGAIN;
	}

	s->value++;
	return 0;
}

int sem_tryenter(sem_t *s) {
	int err;
	sched_lock();
	{
		err = tryenter_sched_lock(s);
	}
	sched_unlock();
	return err;
}

void sem_leave(sem_t *s) {
	assert(s);
	assert(!critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK)));

	sched_lock();
	{
		s->value--;
		wait_queue_notify(&s->wq);
	}
	sched_unlock();
}
