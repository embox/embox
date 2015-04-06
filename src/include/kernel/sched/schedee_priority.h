/**
 * @file
 * @brief
 *
 * @date 21.11.2013
 * @author Andrey Kokorev
 */

#ifndef STRATEGY_SCHEDEE_PRIORITY_H_
#define STRATEGY_SCHEDEE_PRIORITY_H_

struct schedee;
struct schedee_priority;

#include <kernel/sched/sched_priority.h>

#include <module/embox/kernel/sched/priority/priority.h>

extern int schedee_priority_init(struct schedee *p, sched_priority_t new_priority);
extern int schedee_priority_set(struct schedee *p, sched_priority_t new_priority);
extern sched_priority_t schedee_priority_get(struct schedee *p);
extern sched_priority_t schedee_priority_inherit(struct schedee *p,
		sched_priority_t priority);
extern sched_priority_t schedee_priority_reverse(struct schedee *p);

#endif /* STRATEGY_SCHEDEE_PRIORITY_H_ */
