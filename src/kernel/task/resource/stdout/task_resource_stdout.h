/**
 * @file
 *
 * @date Feb 3, 2021
 * @author Anton Bondarev
 */

#ifndef SRC_KERNEL_TASK_RESOURCE_STDOUT_TASK_RESOURCE_STDOUT_H_
#define SRC_KERNEL_TASK_RESOURCE_STDOUT_TASK_RESOURCE_STDOUT_H_

#include <stdio.h>

struct task;

extern FILE *task_resource_sdtout(const struct task *task);
extern FILE *task_self_resource_sdtout(void);

#endif /* SRC_KERNEL_TASK_RESOURCE_STDOUT_TASK_RESOURCE_STDOUT_H_ */
