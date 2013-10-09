/**
 * @file
 *
 * @date Aug 2, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/task.h>

#include <kernel/thread/thread_local.h>
#include <kernel/task/thread_key_table.h>

int task_add_thread(struct task * task, struct thread *t) {
	sched_priority_t sched_prior;

	if ((NULL == task) || (NULL == t)) {
		return -EINVAL;
	}

	/* insert new t to the list */
	dlist_head_init(&t->thread_link);
	dlist_add_next(&t->thread_link, &task->main_thread->thread_link);

	t->task = task;

	/* we initialize thread priority for default task priority and now we must
	 * rescheduler thread
	 */
	sched_prior = sched_priority_full(task->priority, thread_priority_get(t));
	thread_priority_set(t, sched_prior);

	if (-ENOMEM == thread_local_alloc(t, THREAD_KEYS_QUANTITY)) {
		return -ENOMEM;
	}

	return ENOERR;
}


int task_remove_thread(struct task * task, struct thread *thread) {
	if((NULL == task) || (NULL == thread)) {
		return -EINVAL;
	}

	if(NULL == task->main_thread) {
		return -EINVAL;
	}

	if(task->main_thread == thread) {
		return -EBUSY;
	}
#if 0
	if(dlist_empty(&task->main_thread->thread_link)) {
		return -EBUSY;
	}
#endif

	dlist_del(&thread->thread_link);

	thread_local_free(thread);

	return ENOERR;
}


