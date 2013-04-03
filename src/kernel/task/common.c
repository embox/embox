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
#include <kernel/cpu.h>
#include <linux/list.h>
#include "common.h"

#include <module/embox/kernel/task/api.h>

EMBOX_UNIT_INIT(kernel_task_init);

#define MAX_RES_SUM_SIZE OPTION_MODULE_GET(embox__kernel__task__api, NUMBER, max_resource_size)

struct kernel_task {
	struct task task;
	char resource[MAX_RES_SUM_SIZE];
};

static struct kernel_task kernel_task;

ARRAY_SPREAD_DEF(const struct task_resource_desc *, task_resource_desc_array);
ARRAY_SPREAD_DEF(const task_notifing_resource_hnd, task_notifing_resource);

static size_t resource_sum_size = 0;

size_t task_resource_sum_size(void) {
	return resource_sum_size;
}

size_t task_size(void) {
	return sizeof(struct task) + resource_sum_size;
}

struct task *task_kernel_task(void) {
	return &kernel_task.task;
}

struct task *task_init(void *task_n_res_space, size_t size) {
	struct task *task = (struct task *) task_n_res_space;
	void *res_ptr = task_n_res_space + sizeof(struct task);
	const struct task_resource_desc *res_desc;
	size_t task_sz = task_size();

	if (size < task_sz) {
		return NULL;
	}

	memset(task_n_res_space, 0, task_sz);

	INIT_LIST_HEAD(&task->threads);
	INIT_LIST_HEAD(&task->children);
	INIT_LIST_HEAD(&task->link);

	task->parent = NULL;

	task->priority = TASK_PRIORITY_DEFAULT;

	task_resource_foreach(res_desc) {
		res_desc->init(task, res_ptr);
		res_ptr += res_desc->resource_size;
	}

	/* FIXME: This should be only in SMP */
	task->affinity = (1 << NCPU) - 1;

	return task;

}

static inline void resource_sum_size_calc(void) {
	const struct task_resource_desc *res_desc;

	task_resource_foreach(res_desc) {
		resource_sum_size += res_desc->resource_size;
	}
}


static int kernel_task_init(void) {
	resource_sum_size_calc();

	if (!task_init(task_kernel_task(), sizeof(kernel_task))) {
		return -ENOMEM;
	}

	strcpy(task_kernel_task()->name, "kernel");

	return 0;
}
