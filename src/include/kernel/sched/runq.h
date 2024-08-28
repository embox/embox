/**
 * @file
 *
 * @date Jul 29, 2013
 * @author: Anton Bondarev
 */

#ifndef SCHED_RUNQ_H_
#define SCHED_RUNQ_H_


#include <module/embox/kernel/sched/strategy/runq/api.h>

struct schedee;

extern void runq_init(runq_t *queue);
extern void runq_insert(runq_t *queue, struct schedee *schedee);
extern void runq_remove(runq_t *queue, struct schedee *schedee);
extern struct schedee *runq_extract(runq_t *queue);
extern struct schedee *runq_get_next(runq_t *queue);
extern struct schedee *runq_get_next_ignore_affinity(runq_t *queue);

extern void runq_item_init(runq_item_t *runq_link);

#define RUNQ_ITEM_INIT(item) \
	__RUNQ_ITEM_INIT(item)

#endif /* SCHED_RUNQ_H_ */
