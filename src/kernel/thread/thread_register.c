/**
 * @file
 *
 * @date Aug 2, 2013
 * @author: Anton Bondarev
 */

#include <util/dlist.h>
#include <assert.h>
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/thread/thread_register.h>
#include <kernel/thread/priority_priv.h>

void thread_register(struct task *tsk, struct thread *t) {
	sched_priority_t sched_prior;

	assert(tsk != NULL);
	assert(t != NULL);

	/* insert new t to the list */
	dlist_head_init(&t->thread_link);
	dlist_add_next(&t->thread_link, &task_get_main(tsk)->thread_link);

	t->task = tsk;

	/* we initialize thread priority for default task priority and now we must
	 * rescheduler thread
	 */
	//sched_prior = sched_priority_full(task->priority, thread_priority_get(t));
	sched_prior = thread_priority_get(t);
	thread_priority_set(t, sched_prior);
}


void thread_unregister(struct task *tsk, struct thread *t) {
	assert(tsk != NULL);
	assert(t != NULL);
	assert(task_get_main(tsk) != NULL);

	if (t != task_get_main(tsk)) {
		dlist_del(&t->thread_link);
	}
}
