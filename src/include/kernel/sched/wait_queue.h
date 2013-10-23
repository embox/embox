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

extern void wait_queue_thread_notify(struct thread *thread, int result);
extern void wait_queue_remove(struct wait_link *wait_link);
extern void wait_queue_insert(struct wait_queue *wait_queue,
		struct wait_link *wait_link);
extern void wait_queue_prepare(struct wait_link *wait_link);
extern void wait_queue_cleanup(struct wait_link *wait_link);

extern int wait_queue_wait(struct wait_queue *wait_queue, int timeout);
extern int wait_queue_wait_locked(struct wait_queue *wait_queue, int timeout);
extern void wait_queue_notify(struct wait_queue *wait_queue);
extern void wait_queue_notify_all_err(struct wait_queue *wait_queue, int error);

static inline void wait_queue_notify_all(struct wait_queue *wait_queue) {
	wait_queue_notify_all_err(wait_queue, 0);
}

__END_DECLS

#endif /* KERNEL_SCHED_WAIT_QUEUE_H_ */
