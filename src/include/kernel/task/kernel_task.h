/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef KERNEL_TASK_KERNEL_TASK_H_
#define KERNEL_TASK_KERNEL_TASK_H_

#include <sys/cdefs.h>

struct task;

__BEGIN_DECLS

/**
 * @brief Kernel task
 *
 * @return Pointer to kernel task
 */
extern struct task *task_kernel_task(void);

__END_DECLS

#endif /* KERNEL_TASK_KERNEL_TASK_H_ */
