/**
 * @file
 *
 * @date Nov 13, 2013
 * @author: Anton Bondarev
 */

#ifndef KERNEL_TASK_RESOURCE_H_
#define KERNEL_TASK_RESOURCE_H_

#include <stddef.h>
#include <util/array.h>

struct task;
struct thread;

struct task_resource_desc {
	void (*init)(const struct task *task, void *resource_space);
	int (*inherit)(const struct task *task,
			const struct task *parent);
	void (*deinit)(const struct task *task);
	size_t resource_size; /* to be used in on-stack allocation */
	size_t *resource_offset;
};

typedef int (*task_notifing_resource_hnd)(struct thread *prev, struct thread *next);

#define TASK_RESOURCE_DESC(res) \
	static const struct task_resource_desc res; \
	ARRAY_SPREAD_DECLARE(const struct task_resource_desc *, \
			task_resource_desc_array); \
	ARRAY_SPREAD_ADD(task_resource_desc_array, &res)

#define TASK_RESOURCE_NOTIFY(fn) \
	ARRAY_SPREAD_DECLARE(const task_notifing_resource_hnd, \
			task_notifing_resource); \
	ARRAY_SPREAD_ADD(task_notifing_resource, fn)

extern size_t task_resource_size(void);

extern void task_resource_init(const struct task *task,
		void *resource_space);
extern int task_resource_inherit(const struct task *task,
		const struct task *parent);
extern void task_resource_deinit(const struct task *task);

ARRAY_SPREAD_DECLARE(const struct task_resource_desc *,
		task_resource_desc_array);
ARRAY_SPREAD_DECLARE(const task_notifing_resource_hnd,
		task_notifing_resource);

#define task_resource_foreach(item) \
	array_spread_foreach(item, task_resource_desc_array)

#define task_notifing_resource_foreach(item) \
	array_spread_foreach(item, task_notifing_resource)

#endif /* KERNEL_TASK_RESOURCE_H_ */
