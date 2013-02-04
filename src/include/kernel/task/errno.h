/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.06.2012
 */

#ifndef KERNEL_TASK_H_
#define KERNEL_TASK_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern int * task_self_errno_ptr(void);

struct task;

int task_errno(struct task *task);

void task_errno_set(struct task *task, int errno);

__END_DECLS

#endif /* KERNEL_TASK_H_ */
