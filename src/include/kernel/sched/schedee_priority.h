/**
 * @file
 * @brief TODO documentation for sched_priority.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef STRATEGY_SCHEDEE_PRIORITY_H_
#define STRATEGY_SCHEDEE_PRIORITY_H_

#include <module/embox/kernel/sched/priority/priority.h>

#define SCHED_PRIORITY_MIN (0)
#define SCHED_PRIORITY_MAX 255
#define SCHED_PRIORITY_TOTAL \
	(SCHED_PRIORITY_MAX - SCHED_PRIORITY_MIN + 1)

#define SCHED_PRIORITY_NORMAL \
	(SCHED_PRIORITY_MIN + SCHED_PRIORITY_MAX) / 2

#define SCHED_PRIORITY_LOW  \
	(SCHED_PRIORITY_MIN + SCHED_PRIORITY_NORMAL) / 2
#define SCHED_PRIORITY_HIGH \
	(SCHED_PRIORITY_MAX + SCHED_PRIORITY_NORMAL) / 2


struct schedee;
struct schedee_priority;

typedef unsigned int sched_priority_t;


extern int schedee_priority_init(struct schedee *p, sched_priority_t new_priority);
extern int schedee_priority_set(struct schedee *p, sched_priority_t new_priority);
extern sched_priority_t schedee_priority_get(struct schedee *p);
extern sched_priority_t schedee_priority_inherit(struct schedee *p,
		sched_priority_t priority);
extern sched_priority_t schedee_priority_reverse(struct schedee *p);

#endif /* STRATEGY_SCHEDEE_PRIORITY_H_ */
