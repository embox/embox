/**
 * @file
 * @brief TODO documentation for sched_priority.h -- Eldar Abusalimov
 *
 * @date Sep 6, 2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_SCHED_SCHED_PRIORITY_H_
#define KERNEL_SCHED_SCHED_PRIORITY_H_

typedef unsigned int sched_priority_t;

#define SCHED_PRIORITY_MIN (0)
#define SCHED_PRIORITY_MAX 255
#define SCHED_PRIORITY_TOTAL \
	(SCHED_PRIORITY_MAX - SCHED_PRIORITY_MIN + 1)

#include <kernel/sched/schedee_priority.h>

#endif /* KERNEL_SCHED_SCHED_PRIORITY_H_ */
