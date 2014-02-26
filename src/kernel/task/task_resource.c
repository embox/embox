/**
 * @file
 * @brief
 *
 * @date 26.02.14
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <embox/unit.h>
#include <kernel/task/resource.h>
#include <stddef.h>
#include <util/array.h>
#include <util/binalign.h>

EMBOX_UNIT_INIT(task_resource_module_init);

ARRAY_SPREAD_DEF(const struct task_resource_desc *, task_resource_desc_array);
ARRAY_SPREAD_DEF(const task_notifing_resource_hnd, task_notifing_resource);

static size_t resource_sum_size;

size_t task_resource_size(void) {
	return resource_sum_size;
}

void task_resource_init(const struct task *task,
		void *resource_space) {
	const struct task_resource_desc *res_desc;

	task_resource_foreach(res_desc) {
		if (res_desc->init != NULL) {
			res_desc->init(task, resource_space);
		}
		resource_space += res_desc->resource_size;
	}
}

static void task_resource_deinit_before(const struct task *task,
		const struct task_resource_desc *before) {
	const struct task_resource_desc *res_desc;

	task_resource_foreach(res_desc) {
		if (res_desc == before) {
			break;
		}

		if (res_desc->deinit != NULL) {
			res_desc->deinit(task);
		}
	}
}

int task_resource_inherit(const struct task *task,
		const struct task *parent) {
	int ret;
	const struct task_resource_desc *res_desc;

	task_resource_foreach(res_desc) {
		if (res_desc->inherit != NULL) {
			ret = res_desc->inherit(task, parent);
			if (ret != 0) {
				task_resource_deinit_before(parent, res_desc);
				return ret;
			}
		}
	}

	return 0;
}

void task_resource_deinit(const struct task *task) {
	task_resource_deinit_before(task, NULL); /* deinit all */
}

static int task_resource_module_init(void) {
	const struct task_resource_desc *res_desc;

	resource_sum_size = 0;
	task_resource_foreach(res_desc) {
		assert(res_desc->resource_offset != NULL);
		*res_desc->resource_offset = resource_sum_size;

		resource_sum_size += res_desc->resource_size;
	}

	return 0;
}
