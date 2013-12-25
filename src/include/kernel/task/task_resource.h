/**
 * @file
 *
 * @date Nov 13, 2013
 * @author: Anton Bondarev
 */

#ifndef TASK_RESOURCE_H_
#define TASK_RESOURCE_H_

#include <stddef.h>

struct task;
struct thread;

struct task_resource_desc {
	void (*init)(struct task *task, void *resource_space);
	int (*inherit)(struct task *task, struct task *parent_task);
	void (*deinit)(struct task *task);
	size_t resource_size; /* to be used in on-stack allocation */
	int (*resource_init)(const struct task_resource_desc *desc, size_t offset);
	//void (*resource_get)(struct task);
};

typedef int (*task_notifing_resource_hnd)(struct thread *prev, struct thread *next);

#define TASK_RESOURCE_DESC(res) \
	ARRAY_SPREAD_DECLARE(const struct task_resource_desc *, \
			task_resource_desc_array); \
	ARRAY_SPREAD_ADD(task_resource_desc_array, res)

#define TASK_RESOURCE_NOTIFY(fn) \
	ARRAY_SPREAD_DECLARE(const task_notifing_resource_hnd, \
			task_notifing_resource); \
	ARRAY_SPREAD_ADD(task_notifing_resource, fn)



#endif /* TASK_RESOURCE_H_ */
