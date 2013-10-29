/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHED_WAIT_QUEUE_H_
#define KERNEL_SCHED_WAIT_QUEUE_H_

#include <util/dlist.h>

#include <kernel/work.h>

struct wait_queue {
	struct dlist_head list;
	int flag;
};

struct thread;
struct wait_link {
	struct dlist_head link;
	struct thread *thread;
	int result;
};

#define WAIT_QUEUE_INIT(wq)  {.list = DLIST_INIT(wq.list),.flag = 0}

#include <sys/cdefs.h>
__BEGIN_DECLS

static inline void wait_queue_init(struct wait_queue *wait_queue) {
	dlist_init(&wait_queue->list);
}

static inline int wait_queue_empty(struct wait_queue *wait_queue) {
	return dlist_empty(&wait_queue->list);
}

/**
 * Removing wait_link from queue. Sched uses it for finishing sleeping thread
 * @param wait_link
 *   wait_link to remove
 */
extern void wait_queue_remove(struct wait_link *wait_link);

/**
 * Wait notifying with sched locking
 * @param wait_queue
 *   queue with waiting threads
 * @param timeout
 *   timeout for interrupt waiting
 * @return
 *   waiting result
 */
extern int wait_queue_wait(struct wait_queue *wait_queue, int timeout);
/**
 * Wait notifying without sched locking
 * @param wait_queue
 *   queue with waiting threads
 * @param timeout
 *   timeout for interrupt waiting
 * @return
 *   waiting result
 */
extern int wait_queue_wait_locked(struct wait_queue *wait_queue, int timeout);
/**
 * Notifying specific thread
 * @param thread
 *   thread for notify
 * @param result
 *   result returns from wait_queue_wait(_locked) funtions
 */
extern void wait_queue_thread_notify(struct thread *thread, int result);
/**
 * Notify the most significant thread in queue
 * @param wait_queue
 *   queue with threads to notify
 */
extern void wait_queue_notify(struct wait_queue *wait_queue);
/**
 * Notify all threads in queue with error
 * @param wait_queue
 *   queue with threads to notify
 * @param error
 *   code error
 */
extern void wait_queue_notify_all_err(struct wait_queue *wait_queue, int error);

static inline void wait_queue_notify_all(struct wait_queue *wait_queue) {
	wait_queue_notify_all_err(wait_queue, 0);
}

__END_DECLS

#endif /* KERNEL_SCHED_WAIT_QUEUE_H_ */
