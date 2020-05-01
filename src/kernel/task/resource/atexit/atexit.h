/**
 * @file
 *
 * @date May 1, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_KERNEL_TASK_RESOURCE_ATEXIT_ATEXIT_H_
#define SRC_KERNEL_TASK_RESOURCE_ATEXIT_ATEXIT_H_

#include <limits.h>

#include <kernel/task.h>

struct task_atexit {
	void (*atexit_func[ATEXIT_MAX])(void);
};

__BEGIN_DECLS

extern struct task_atexit *task_resource_atexit(const struct task *task);

#define task_self_resource_atexit() task_resource_atexit(task_self())

__END_DECLS


#endif /* SRC_KERNEL_TASK_RESOURCE_ATEXIT_ATEXIT_H_ */
