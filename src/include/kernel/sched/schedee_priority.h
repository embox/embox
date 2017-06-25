/**
 * @file
 * @brief TODO documentation for sched_priority.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef SCHED_SCHEDEE_PRIORITY_H_
#define SCHED_SCHEDEE_PRIORITY_H_

#include <module/embox/kernel/sched/priority/priority.h>

#define SCHED_PRIORITY_MIN (0)
#define SCHED_PRIORITY_MAX 255
#define SCHED_PRIORITY_TOTAL \
	(SCHED_PRIORITY_MAX - SCHED_PRIORITY_MIN + 1)

/** Default priority value for #thread */
#define SCHED_PRIORITY_NORMAL \
	(SCHED_PRIORITY_MIN + SCHED_PRIORITY_MAX) / 2

#define SCHED_PRIORITY_LOW  \
	(SCHED_PRIORITY_MIN + SCHED_PRIORITY_NORMAL) / 2
/** Default priority value for #lthread */
#define SCHED_PRIORITY_HIGH \
	(SCHED_PRIORITY_MAX + SCHED_PRIORITY_NORMAL) / 2


struct schedee;
struct schedee_priority;

/**
 * Initialize #schedee_priority of @p schedee with @p new_priority.
 */
extern int schedee_priority_init(struct schedee *schedee, int new_priority);

/**
 * Sets @p new_priority to #schedee_priority of @p schedee.
 */
extern int schedee_priority_set(struct schedee *schedee, int new_priority);

/**
 * Returns @p schedee priority.
 */
extern int schedee_priority_get(struct schedee *schedee);

/**
 * Inherits priority in order to prevent the priority inversion.
 * @param  schedee  The schedee which priority has to inherit @p priority.
 * @param  priority The priority to inherit.
 * @return          The new priority of @p schedee.
 */
extern int schedee_priority_inherit(struct schedee *schedee, int priority);

/**
 * Reverses the previously inherited priority of @p schedee.
 * @param  schedee The schedee which priority has to be reversed.
 * @return         The new priority of @p schedee.
 */
extern int schedee_priority_reverse(struct schedee *schedee);

#endif /* SCHED_SCHEDEE_PRIORITY_H_ */
