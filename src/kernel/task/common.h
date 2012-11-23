/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.06.2012
 */

#include <kernel/task.h>
#include <util/array.h>

struct thread;

struct task_resource_desc {
	void (*init)(struct task *task, void *resource_space);
	void (*inherit)(struct task *task, struct task *parent_task);
	void (*deinit)(struct task *task);
	size_t resource_size; /* to be used in on-stack allocation */
};

typedef int (*task_notifing_resource_hnd)(struct thread *prev, struct thread *next);

extern const struct task_resource_desc *task_resource_desc_array[];

extern const task_notifing_resource_hnd task_notifing_resource[];

extern size_t task_kernel_size(void);

extern size_t task_resource_sum_size(void);

extern struct task *task_init(void *task_n_res_space, size_t size);

#define TASK_RESOURCE_DESC(...) \
	ARRAY_SPREAD_ADD(task_resource_desc_array, __VA_ARGS__)

#define task_resource_foreach(item) \
	array_foreach(item, task_resource_desc_array, \
		ARRAY_SPREAD_SIZE(task_resource_desc_array))

#define TASK_RESOURCE_NOTIFY(fn) \
	ARRAY_SPREAD_ADD(task_notifing_resource, fn)

#define task_notifing_resource_foreach(item) \
	array_foreach(item, task_notifing_resource, \
		ARRAY_SPREAD_SIZE(task_notifing_resource))
