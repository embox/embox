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

struct wait_link {
	struct dlist_head link;
	struct thread *thread;
};

static void on_notified(struct thread *thread, void *data) {
	struct wait_link *wait_link = data;

	dlist_del(&wait_link->link);
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

	dlist_head_init(&wait_link.link);
	wait_link.thread = thread_self();

	IPL_SAFE_DO(dlist_add_prev(&wait_link.link, &wait_queue->list));
	sched_prepare_wait(&on_notified, &wait_link);
	result = sched_wait_locked(timeout);
	sched_cleanup_wait();

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
