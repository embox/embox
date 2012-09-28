/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <embox/unit.h>
#include <kernel/task.h>
#include <lib/list.h>
#include "common.h"

#include <mem/vmem.h>
#include <hal/mm/mmu_core.h>

EMBOX_UNIT_INIT(tasks_init);

#include <module/embox/kernel/task/api.h>

#define MAX_RES_SUM_SIZE OPTION_MODULE_GET(embox__kernel__task__api, NUMBER, max_resource_size)

static char kernel_task[sizeof(struct task) + MAX_RES_SUM_SIZE];

ARRAY_SPREAD_DEF(const struct task_resource_desc *, task_resource_desc_array);
ARRAY_SPREAD_DEF(const task_notifing_resource_hnd, task_notifing_resource);

static size_t resource_sum_size = 0;

size_t task_resource_sum_size(void) {
	return resource_sum_size;
}

struct task *task_init(void *task_n_res_space) {
	struct task *task = (struct task *) task_n_res_space;
	void *res_ptr = task_n_res_space + sizeof(struct task);
	const struct task_resource_desc *res_desc;

	memset(task_n_res_space, 0, sizeof(struct task) + resource_sum_size);

	INIT_LIST_HEAD(&task->threads);
	INIT_LIST_HEAD(&task->children);
	INIT_LIST_HEAD(&task->link);

	task->parent = NULL;

	task->ctx = mmu_create_context();
	vmem_create_virtual_space(task->ctx);

	task_resource_foreach(res_desc) {
		res_desc->init(task, res_ptr);
		res_ptr += res_desc->resource_size;
	}

	return task;

}

struct task *task_kernel_task(void) {
	return (struct task *) kernel_task;
}

static int tasks_init(void) {
	const struct task_resource_desc *res_desc;

	task_resource_foreach(res_desc) {
		resource_sum_size += res_desc->resource_size;
	}

	if (MAX_RES_SUM_SIZE < resource_sum_size) {
		return -ENOMEM;
	}

	task_init(kernel_task);

	return 0;
}
