/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.06.2012
 */

#include <kernel/task.h>
#include <util/array.h>

struct task_resource_desc {
	void (*init)(struct task *task);
	void (*inherit)(struct task *task, struct task *parent_task);
	size_t resource_size; /* to be used in on-stack allocation */
};

extern const struct task_resource_desc *task_resource_desc_array[];

#define TASK_RESOURCE_DESC(...) \
	ARRAY_SPREAD_ADD(task_resource_desc_array, __VA_ARGS__)

#define task_resource_foreach(item) \
	array_foreach(item, task_resource_desc_array, \
		ARRAY_SPREAD_SIZE(task_resource_desc_array))
