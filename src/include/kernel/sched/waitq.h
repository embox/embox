/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_SCHED_WAITQ_H_
#define KERNEL_SCHED_WAITQ_H_

#include <util/dlist.h>

#include <kernel/work.h>

struct waitq {
	struct dlist_head list;
	int flag;
};

struct thread;
struct wait_link {
	struct dlist_head link;
	struct thread *thread;
	int result;
};

#define WAITQ_INIT(wq)  {.list = DLIST_INIT(wq.list),.flag = 0}

#include <sys/cdefs.h>
__BEGIN_DECLS

static inline void waitq_init(struct waitq *waitq) {
	dlist_init(&waitq->list);
}

static inline int waitq_empty(struct waitq *waitq) {
	return dlist_empty(&waitq->list);
}

/**
 * Removing wait_link from queue. Sched uses it for finishing sleeping thread
 * @param wait_link
 *   wait_link to remove
 */
extern void waitq_remove(struct wait_link *wait_link);

/**
 * Wait notifying with sched locking
 * @param waitq
 *   queue with waiting threads
 * @param timeout
 *   timeout for interrupt waiting
 * @return
 *   waiting result
 */
extern int waitq_wait(struct waitq *waitq, int timeout);
/**
 * Wait notifying without sched locking
 * @param waitq
 *   queue with waiting threads
 * @param timeout
 *   timeout for interrupt waiting
 * @return
 *   waiting result
 */
extern int waitq_wait_locked(struct waitq *waitq, int timeout);
/**
 * Notifying specific thread
 * @param thread
 *   thread for notify
 * @param result
 *   result returns from waitq_wait(_locked) funtions
 */
extern void waitq_thread_notify(struct thread *thread, int result);
/**
 * Notify the most significant thread in queue
 * @param waitq
 *   queue with threads to notify
 */
extern void waitq_notify(struct waitq *waitq);
/**
 * Notify all threads in queue with error
 * @param waitq
 *   queue with threads to notify
 * @param error
 *   code error
 */
extern void waitq_notify_all_err(struct waitq *waitq, int error);


/* You should use function with __waitq prefix if race condition is possible by
 * using waitq_* functions*/
/**
 * Preparing to wait. Adds wait_link to waitq, puts waiting flag to thread state
 * @param waitq
 *   queue with threads to notify
 * @param wait_link
 *   wait_link to remove
 */
extern void __waitq_prepare(struct waitq *waitq, struct wait_link *wait_link);
/**
 * Cleanup thread->wait_link, must be used after each waiting
 */
extern void __waitq_cleanup(void);
/**
 * Waiting with sched locking, needs preparations by __waitq_prepare.
 * @param timeout
 *   timeout for interrupt waiting
 * @return
 *   waiting result
 */
extern int __waitq_wait(int timeout);
/**
 * Waiting without sched locking, needs preparations by __waitq_prepare.
 * @param timeout
 *   timeout for interrupt waiting
 * @return
 *   waiting result
 */
extern int __waitq_wait_locked(int timeout);

static inline void waitq_notify_all(struct waitq *waitq) {
	waitq_notify_all_err(waitq, 0);
}

__END_DECLS

#endif /* KERNEL_SCHED_WAITQ_H_ */
