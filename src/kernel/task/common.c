/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */

#include <embox/unit.h>

#include <errno.h>
#include <string.h>

#include <kernel/task.h>
#include <lib/list.h>
#include "common.h"

#include <module/embox/kernel/task/api.h>

EMBOX_UNIT_INIT(kernel_task_init);

#define MAX_RES_SUM_SIZE OPTION_MODULE_GET(embox__kernel__task__api, NUMBER, max_resource_size)

static char kernel_task[sizeof(struct task) + MAX_RES_SUM_SIZE];

ARRAY_SPREAD_DEF(const struct task_resource_desc *, task_resource_desc_array);
ARRAY_SPREAD_DEF(const task_notifing_resource_hnd, task_notifing_resource);

static size_t resource_sum_size = 0;

size_t task_resource_sum_size(void) {
	return resource_sum_size;
}

size_t task_kernel_size(void) {
	return sizeof(struct task) + MAX_RES_SUM_SIZE;
}

struct task *task_kernel_task(void) {
	return (struct task *) kernel_task;
}

struct task *task_init(void *task_n_res_space, size_t size) {
	struct task *task = (struct task *) task_n_res_space;
	void *res_ptr = task_n_res_space + sizeof(struct task);
	const struct task_resource_desc *res_desc;

	if (resource_sum_size == 0) {
		task_resource_foreach(res_desc) {
			resource_sum_size += res_desc->resource_size;
		}

	}

	if (size <= resource_sum_size) {
		return NULL;
	}

	memset(task_n_res_space, 0, sizeof(struct task) + resource_sum_size);

	INIT_LIST_HEAD(&task->threads);
	INIT_LIST_HEAD(&task->children);
	INIT_LIST_HEAD(&task->link);

	task->parent = NULL;

	task_resource_foreach(res_desc) {
		res_desc->init(task, res_ptr);
		res_ptr += res_desc->resource_size;
	}

	return task;

}

static int kernel_task_init(void) {
	if (!task_init(task_kernel_task(), task_kernel_size())) {
		return -ENOMEM;
	}

	return 0;
}
