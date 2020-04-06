/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 * @author: Anton Kozlov
 */

#ifndef SCHED_PRIORITY_INHERIT_H_
#define SCHED_PRIORITY_INHERIT_H_

struct schedee_priority {
	short base_priority;      /**< Priority was set for the schedee. */
	short current_priority;   /**< Current schedee scheduling priority. */
};

#define __SCHED_PRIORITY_INIT(prio) \
	{ .base_priority = prio, .current_priority = prio }

#endif /* SCHED_PRIORITY_INHERIT_H_ */
