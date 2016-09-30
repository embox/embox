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

#endif /* SCHED_PRIORITY_INHERIT_H_ */
