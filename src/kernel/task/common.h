/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.06.2012
 */

#include <kernel/task.h>

extern size_t task_resource_sum_size(void);
extern size_t task_size(void);

extern struct task *task_init(void *task_n_res_space, size_t size);

ARRAY_SPREAD_DECLARE(const struct task_resource_desc *,
		task_resource_desc_array);
ARRAY_SPREAD_DECLARE(const task_notifing_resource_hnd,
		task_notifing_resource);

#define task_resource_foreach(item) \
	array_spread_foreach(item, task_resource_desc_array)

#define task_notifing_resource_foreach(item) \
	array_spread_foreach(item, task_notifing_resource)
