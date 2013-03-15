/**
 * @file
 * @brief
 *
 * @date 13.03.2013
 * @author Anton Bulychev
 */

#ifndef TASK_PRIORITY_H_
#define TASK_PRIORITY_H_

#define TASK_PRIORITY_DEFAULT   0
#define TASK_PRIORITY_MIN     -20
#define TASK_PRIORITY_MAX      19
#define TASK_PRIORITY_TOTAL \
	(TASK_PRIORITY_MAX - TASK_PRIORITY_MIN + 1)

typedef short task_priority_t;

#endif /* TASK_PRIORITY_H_ */
