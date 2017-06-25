/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#ifndef KERNEL_TASK_RESOURCE_ERRNO_H_
#define KERNEL_TASK_RESOURCE_ERRNO_H_

#include <sys/cdefs.h>

struct task;

__BEGIN_DECLS

extern int * task_resource_errno(const struct task *task);
extern int * task_self_resource_errno(void); // FIXME
//#define task_self_resource_errno() task_resource_errno(task_self())

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_ERRNO_H_ */
