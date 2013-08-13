/**
 * @file
 *
 * @date Jul 29, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_RUNQ_H_
#define SCHED_RUNQ_H_


#include <module/embox/kernel/sched/strategy/runq_api.h>

struct thread;

extern void runq_queue_init(runq_queue_t *queue);
extern void runq_queue_insert(runq_queue_t *queue, struct thread *thread);
extern void runq_queue_remove(runq_queue_t *queue, struct thread *thread);
extern struct thread *runq_queue_extract(runq_queue_t *queue);

extern void runq_item_init(runq_item_t *runq_link);

#endif /* SCHED_RUNQ_H_ */
