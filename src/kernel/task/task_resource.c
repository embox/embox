/**
 * @file
 * @brief
 *
 * @date 26.02.14
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>

#include <lib/libds/array.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(task_resource_module_init);

ARRAY_SPREAD_DEF(const struct task_resource_desc *const, task_resource_desc_array);

void task_resource_init(const struct task *task) {
	const struct task_resource_desc *res;

	task_resource_foreach(res) {
		assert(res->resource_offset != NULL);
		assert(binalign_check_bound(
				*res->resource_offset, sizeof(void *)));
		if (res->init != NULL) {
			res->init(task, (void *) task->resources + *res->resource_offset);
		}
	}
}

void task_resource_exec(const struct task *task, const char* path, char *const argv[]) {
	const struct task_resource_desc *res;

	task_resource_foreach(res) {
		if (res->exec != NULL) {
			res->exec(task, path, argv);
		}
	}
}

static void task_resource_deinit_before(const struct task *task,
		const struct task_resource_desc *before) {
	const struct task_resource_desc *res;

	task_resource_foreach(res) {
		if (res == before) {
			break;
		}

		if (res->deinit != NULL) {
			res->deinit(task);
		}
	}
}

int task_resource_inherit(const struct task *task, const struct task *parent) {
	int ret;
	const struct task_resource_desc *res;

	assert(task);
	assert(parent);

	task_resource_foreach(res) {
		if (res->inherit != NULL) {
			ret = res->inherit(task, parent);

			if (ret != 0) {
				task_resource_deinit_before(parent, res);
				return ret;
			}
		}
	}

	return 0;
}

void task_resource_deinit(const struct task *task) {
	assert(task);

	task_resource_deinit_before(task, NULL); /* deinit all */
}


static int task_resource_module_init(void) {
	size_t offset;
	const struct task_resource_desc *res;

	offset = 0;

	task_resource_foreach(res) {
		assert(res->resource_offset);
		*res->resource_offset = offset;

		assert((offset + res->resource_size) <= TASK_RESOURCE_SIZE);
		offset += binalign_bound(res->resource_size, sizeof(void *));
	}

	assert(offset == binalign_bound(TASK_RESOURCE_SIZE, sizeof(void *)));

	return 0;
}
