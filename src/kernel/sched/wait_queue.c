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
#include <kernel/sched.h>
#include <kernel/sched/wait_queue.h>
#include <kernel/sched_wait.h>

#include <util/member.h>
#include <util/dlist.h>

static void on_notified(struct thread *thread, void *data) {
	struct wait_link *wait_link = data;

	ipl_t ipl = ipl_save();
	{
		dlist_del(&wait_link->link);
	}
	ipl_restore(ipl);
}

void wait_queue_insert(struct wait_queue *wait_queue,
		struct wait_link *wait_link) {
	ipl_t ipl = ipl_save();
	{
		dlist_head_init(&wait_link->link);
		wait_link->thread = thread_self();

		dlist_add_prev(&wait_link->link, &wait_queue->list);
	}
	ipl_restore(ipl);
}

void wait_queue_prepare(struct wait_link *wait_link) {
	sched_prepare_wait(&on_notified, wait_link);
}

void wait_queue_cleanup(struct wait_link *wait_link) {
	//TODO: delete this
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

	wait_queue_insert(wait_queue, &wait_link);

	wait_queue_prepare(&wait_link);

	result = sched_wait_locked(timeout);

	wait_queue_cleanup(&wait_link);

	return result;
}


void wait_queue_notify(struct wait_queue *wait_queue) {
	struct wait_link *link, *next;
	struct thread *t;

	ipl_t ipl = ipl_save();
	{
		if (dlist_empty(&wait_queue->list)) {
			goto out;
		}
		t = dlist_entry(wait_queue->list.next, struct wait_link, link)->thread;

		dlist_foreach_entry(link, next, &wait_queue->list, link) {
			if (thread_priority_get(link->thread) > thread_priority_get(t)) {
				t = link->thread;
			}
		}

		sched_thread_notify(t, ENOERR);
	}
out:
	ipl_restore(ipl);
}

void wait_queue_notify_all_err(struct wait_queue *wait_queue, int error) {
	struct wait_link *link, *next;

	ipl_t ipl = ipl_save();
	{
		dlist_foreach_entry(link, next, &wait_queue->list, link) {
			sched_thread_notify(link->thread, error);
		}
	}
	ipl_restore(ipl);
}
