/**
 * @file
 * @brief
 *
 * @date 21.11.2013
 * @author Andrey Kokorev
 */

#ifndef STRATEGY_SCHEDEE_PRIORITY_H_
#define STRATEGY_SCHEDEE_PRIORITY_H_

#include <kernel/sched/sched_priority.h>

struct schedee_priority {
	short base_priority;      /**< Priority was set for the schedee. */
	short current_priority;   /**< Current schedee scheduling priority. */
};

typedef struct schedee_priority schedee_priority_t;

extern int schedee_priority_init(struct schedee *s, sched_priority_t new_priority);
extern int schedee_priority_set(struct schedee *s, sched_priority_t new_priority);
extern sched_priority_t schedee_priority_get(struct schedee *s);
extern sched_priority_t schedee_priority_inherit(struct schedee *s,
		sched_priority_t priority);
extern sched_priority_t schedee_priority_reverse(struct schedee *s);

#endif /* STRATEGY_SCHEDEE_PRIORITY_H_ */
