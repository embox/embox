/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#include <errno.h>

#include <hal/ipl.h>

#include <kernel/thread.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/wait_queue.h>

#include <util/member.h>

static void on_notified(struct thread *thread, void *data) {
	struct wait_link *wait_link = data;

	dlist_del(&wait_link->link);
}

void wait_queue_insert(struct wait_queue *wait_queue,
		struct wait_link *wait_link) {

	dlist_head_init(&wait_link->link);
	wait_link->thread = thread_self();

	dlist_add_prev(&wait_link->link, &wait_queue->list);
}

void wait_queue_prepare(struct wait_link *wait_link) {
	sched_prepare_wait(&on_notified, wait_link);
}

void wait_queue_cleanup(struct wait_link *wait_link) {
	sched_cleanup_wait();
}

int wait_queue_wait(struct wait_queue *wait_queue, int timeout) {
	int result;

	sched_lock();
	{
		result = wait_queue_wait_locked(wait_queue, timeout);
	}
	sched_unlock();

	return result;
}

int wait_queue_wait_locked(struct wait_queue *wait_queue, int timeout) {
	struct wait_link wait_link;
	int result;

	IPL_SAFE_DO(wait_queue_insert(wait_queue, &wait_link));

	wait_queue_prepare(&wait_link);

	result = sched_wait_locked(timeout);

	wait_queue_cleanup(&wait_link);

	return result;
}

void wait_queue_notify(struct wait_queue *wait_queue) {
	struct wait_link *link, *next;
	struct thread *thread = NULL;

	ipl_t ipl = ipl_save();
	{
		dlist_foreach_entry(link, next, &wait_queue->list, link) {
			if (thread == NULL ||
					link->thread->sched_priority > thread->sched_priority) {
				thread = link->thread;
			}
		}

		sched_thread_notify(thread, ENOERR);
	}
	ipl_restore(ipl);
}

void wait_queue_notify_all(struct wait_queue *wait_queue) {
	struct wait_link *link, *next;

	ipl_t ipl = ipl_save();
	{
		dlist_foreach_entry(link, next, &wait_queue->list, link) {
			sched_thread_notify(link->thread, ENOERR);
		}
	}
	ipl_restore(ipl);
}
