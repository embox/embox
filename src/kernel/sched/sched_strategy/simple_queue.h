/**
 * @file
 * @brief
 *
 * @date 06.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_SCHED_STRATEGY_SIMPLE_QUEUE_H_
#define KERNEL_THREAD_SCHED_STRATEGY_SIMPLE_QUEUE_H_

#include <module/embox/kernel/sched/sched_policy/queue/api.h>

struct thread;
struct runq {
	runq_queue_t queue;

};


extern void runq_queue_init(runq_queue_t *queue);
extern void runq_queue_insert(runq_queue_t *queue, struct thread *thread);
extern void runq_queue_remove(runq_queue_t *queue, struct thread *thread);
extern struct thread *runq_queue_extract(runq_queue_t *queue);


#endif /* KERNEL_THREAD_SCHED_STRATEGY_SIMPLE_QUEUE_H_ */
