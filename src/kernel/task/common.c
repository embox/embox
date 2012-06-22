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
#include <kernel/thread/sched_lock.h>
#include <lib/list.h>
#include <kernel/thread/api.h>
#include "common.h"

EMBOX_UNIT_INIT(tasks_init);

#define MAX_RES_SUM_SIZE OPTION_GET(NUMBER, max_resource_size)

static char kernel_task[sizeof(struct task) + MAX_RES_SUM_SIZE];

ARRAY_SPREAD_DEF(const struct task_resource_desc *, task_resource_desc_array);
ARRAY_SPREAD_DEF(const task_notifing_resource_hnd, task_notifing_resource);

static size_t resource_sum_size = 0;
static size_t on_demand_memorize_fn(void);

static size_t (*on_demand_calc)(void) = on_demand_memorize_fn;

static size_t on_demand_get_memorized(void) {
	return resource_sum_size;
}

static size_t on_demand_memorize_fn(void) {
	const struct task_resource_desc *res_desc;

	task_resource_foreach(res_desc) {
		resource_sum_size += res_desc->resource_size;
	}

	on_demand_calc = on_demand_get_memorized;

	return resource_sum_size;
}


size_t task_resource_sum_size(void) {
	return on_demand_calc();
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

	task_resource_foreach(res_desc) {
		res_desc->init(task, res_ptr);
		res_ptr += res_desc->resource_size;
	}

	return task;

}

int task_notify_switch(struct thread *prev, struct thread *next) {
	task_notifing_resource_hnd notify_res;
	int res;

	if (prev->task == next->task) {
		return 0;
	}

	task_notifing_resource_foreach(notify_res) {
		if (0 != (res = notify_res(prev, next))) {
			return res;
		}
	}

	return 0;
}

struct task *task_kernel_task(void) {
	return (struct task *) kernel_task;
}

static int tasks_init(void) {

	if (MAX_RES_SUM_SIZE < task_resource_sum_size()) {
		return -ENOMEM;
	}

	task_init(kernel_task);

	return 0;
}
