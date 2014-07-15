/**
 * @file
 * @brief
 *
 * @author Ilia Vaprol
 * @date 26.02.14
 */

#ifndef KERNEL_TASK_RESOURCE_AFFINITY_H_
#define KERNEL_TASK_RESOURCE_AFFINITY_H_

#include <sys/cdefs.h>

struct task;

#define TASK_AFFINITY_DEFAULT ((unsigned int)-1)

__BEGIN_DECLS

extern unsigned int * task_resource_affinity(
		const struct task *task);

static inline void task_set_affinity(const struct task *task,
		unsigned int affinity) {
	*task_resource_affinity(task) = affinity;
}

static inline unsigned int task_get_affinity(
		const struct task *task) {
	return *task_resource_affinity(task);
}

__END_DECLS

#endif /* KERNEL_TASK_RESOURCE_AFFINITY_H_ */
