/**
 * @file
 * @brief
 *
 * @date 16.09.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <string.h>
#include <embox/unit.h>
#include <kernel/task/idx.h>
#include <mem/objalloc.h>
#include "common.h"

#include <util/idx_table.h>

OBJALLOC_DEF(idx_res_pool, struct idx_desc, CONFIG_TASKS_RES_QUANTITY);

static void task_idx_table_init(struct task *task, void* _idx_table) {
	util_num_alloc_t *num_alloc = _idx_table;
	util_idx_table_t *idx_table;
	struct task_idx_table *task_idx_table;

	util_num_alloc_init(num_alloc, CONFIG_TASKS_RES_QUANTITY);

	idx_table = (util_idx_table_t *) ((void *) num_alloc +
			UTIL_NUM_ALLOC_CALC(CONFIG_TASKS_RES_QUANTITY));

	util_idx_table_init(idx_table, CONFIG_TASKS_RES_QUANTITY, num_alloc);

	task_idx_table = (struct task_idx_table *) idx_table;
	task->idx_table = task_idx_table;

}

static void task_idx_table_inherit(struct task *task, struct task *parent) {
	struct task_idx_table *idx_table = task_idx_table(task);
	struct task_idx_table *parent_idx_table = task_idx_table(parent);

	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		task_idx_table_set(idx_table, i, task_idx_table_get(parent_idx_table, i));
	}
}

static void task_idx_table_deinit(struct task *task) {
	struct task_idx_table *idx_table = task_idx_table(task);

	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		task_idx_table_unbind(idx_table, i);
	}

	task->idx_table = NULL;
}


/*static*/ struct idx_desc *task_idx_desc_alloc(const struct task_idx_ops *res_ops, void *fd_struct) {
	struct idx_desc *desc = objalloc(&idx_res_pool);
	desc->link_count = 0;

	desc->res_ops = res_ops;
	desc->data.fd_struct = fd_struct;
	return desc;
}

static void task_idx_desc_free(struct idx_desc *desc) {
	objfree(&idx_res_pool, desc);
}

int task_idx_table_first_unbinded(struct task_idx_table *res) {
	return util_idx_table_next_alloc((util_idx_table_t *) &res->idx);
}

int task_idx_table_set(struct task_idx_table *res, int idx, struct idx_desc *desc) {
	struct idx_desc *old_idx = task_idx_table_get(res, idx);
	int ret = 0;
	assert(res);
	if (old_idx) {
		if (0 == task_idx_desc_link_count_add(old_idx, -1)) {
			ret = task_idx_desc_ops(old_idx)->close(task_idx_desc_data(old_idx));
			task_idx_desc_free(old_idx);
		}
	}

	util_idx_table_set((util_idx_table_t *) &res->idx, idx, desc);

	if (desc) {
		task_idx_desc_link_count_add(desc, 1);
	}

	return ret;
}

int task_self_idx_alloc(const struct task_idx_ops *res_ops, void *data) {
	int new_fd;
	struct task_idx_table *self_res = task_self_idx_table();

	if (NULL == data) {
	       return -1;
	}

	if ((new_fd = task_idx_table_first_unbinded(self_res)) < 0) {
		return new_fd;
	}

	task_idx_table_set(self_res, new_fd, task_idx_desc_alloc(res_ops, data));

	return new_fd;
}

static const struct task_resource_desc idx_resource = {
	.init = task_idx_table_init,
	.inherit = task_idx_table_inherit,
	.deinit = task_idx_table_deinit,
	.resource_size = UTIL_IDX_TABLE_CALC(struct idx_desc *, CONFIG_TASKS_RES_QUANTITY) +
		UTIL_NUM_ALLOC_CALC(CONFIG_TASKS_RES_QUANTITY)
};

TASK_RESOURCE_DESC(&idx_resource);
