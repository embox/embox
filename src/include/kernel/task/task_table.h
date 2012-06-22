/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.06.2012
 */

#ifndef KERNEL_TASK_TASK_TABLE_H_
#define KERNEL_TASK_TASK_TABLE_H_

#include <kernel/task.h>

/**
 * @brief Add a task to a task table
 *
 * @param task
 *
 * @return
 */
extern int task_table_add(struct task *task);

/**
 * @brief Get previously added task from a task table
 *
 * @param n
 *
 * @return
 */
extern struct task *task_table_get(int n);

/**
 * @brief Delete task from a task table
 *
 * @param n
 */
extern void task_table_del(int n);

/**
 * @brief Check, if there is a space in task table for another one task
 *
 * @return
 */
extern int task_table_has_space(void);


#endif /* KERNEL_TASK_TASK_TABLE_H_ */

