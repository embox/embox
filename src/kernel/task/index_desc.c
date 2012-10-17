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

OBJALLOC_DEF(idx_pool, struct idx_desc, TASKS_RES_QUANTITY);
OBJALLOC_DEF(idx_data_pool, struct idx_desc_data, TASKS_RES_QUANTITY);

struct idx_desc *task_idx_desc_alloc(struct idx_desc_data *data) {
	struct idx_desc *idx = objalloc(&idx_pool);

	idx->data = data;
	idx->flags = 0;

	data->link_count += 1;

	return idx;
}

int task_idx_desc_free(struct idx_desc *idx) {
	int res;

	if (1 == idx->data->link_count) {
		if (0 != (res = task_idx_data_free(idx))) {
			return res;
		}
	} else {
		idx->data->link_count -= 1;
	}

	objfree(&idx_pool, idx);
	return 0;
}

struct idx_desc_data *task_idx_data_alloc(const struct task_idx_ops *res_ops, void *fd_struct) {
	struct idx_desc_data *idx_data = objalloc(&idx_data_pool);
	idx_data->link_count = 0;
	idx_data->res_ops = res_ops;
	idx_data->fd_struct = fd_struct;
	return idx_data;
}

int task_idx_data_free(struct idx_desc *idx) {
	int res;

	if (0 != (res = idx->data->res_ops->close(idx))) {
		return res;
	}

	objfree(&idx_data_pool, idx->data);
	return 0;
}

int task_idx_table_first_unbinded(struct task_idx_table *res) {
	return util_idx_table_next_alloc((util_idx_table_t *) &res->idx);
}

int task_idx_table_set(struct task_idx_table *res, int idx, struct idx_desc *desc) {
	struct idx_desc *old_idx = task_idx_table_get(res, idx);
	int ret;

	assert(res);

	if (old_idx) {
		if (0 != (ret = task_idx_desc_free(old_idx))) {
			return ret;
		}
	}

	util_idx_table_set((util_idx_table_t *) &res->idx, idx, desc);

	return 0;
}

int task_self_idx_alloc(const struct task_idx_ops *res_ops, void *fd_struct) {
	int new_fd;
	struct task_idx_table *self_res = task_self_idx_table();
	struct idx_desc_data *data;

	if ((new_fd = task_idx_table_first_unbinded(self_res)) < 0) {
		return new_fd;
	}

	data = task_idx_data_alloc(res_ops, fd_struct);

	if (0 != task_idx_table_set(self_res, new_fd, task_idx_desc_alloc(data))) {
		return -1;
	}

	data->read_state.unblock = data->write_state.unblock = NULL;
	data->write_state.op_is_nonblocking = 1;

	return new_fd;
}

static void task_idx_table_init(struct task *task, void* _idx_table) {
	util_num_alloc_t *num_alloc = _idx_table;
	util_idx_table_t *idx_table;
	struct task_idx_table *task_idx_table;

	util_num_alloc_init(num_alloc, TASKS_RES_QUANTITY);

	idx_table = (util_idx_table_t *) ((void *) num_alloc +
			UTIL_NUM_ALLOC_CALC(TASKS_RES_QUANTITY));

	util_idx_table_init(idx_table, TASKS_RES_QUANTITY, num_alloc);

	task_idx_table = (struct task_idx_table *) idx_table;
	task->idx_table = task_idx_table;

}

static void task_idx_table_inherit(struct task *task, struct task *parent) {
	struct task_idx_table *idx_table = task_idx_table(task);
	struct task_idx_table *parent_idx_table = task_idx_table(parent);

	for (int i = 0; i < TASKS_RES_QUANTITY; i++) {
		if (task_idx_table_is_binded(parent_idx_table, i)) {
			struct idx_desc *par_idx = task_idx_table_get(parent_idx_table, i);
			struct idx_desc *d = task_idx_desc_alloc(par_idx->data);
			assert(0 == task_idx_table_set(idx_table, i, d),
					"task_idx_table_init not properly inited idx table");

		}
	}
}

static void task_idx_table_deinit(struct task *task) {
	struct task_idx_table *idx_table = task_idx_table(task);

	for (int i = 0; i < TASKS_RES_QUANTITY; i++) {
		task_idx_table_unbind(idx_table, i);
	}

	task->idx_table = NULL;
}

static const struct task_resource_desc idx_resource = {
	.init = task_idx_table_init,
	.inherit = task_idx_table_inherit,
	.deinit = task_idx_table_deinit,
	.resource_size = UTIL_IDX_TABLE_CALC(struct idx_desc *, TASKS_RES_QUANTITY) +
		UTIL_NUM_ALLOC_CALC(TASKS_RES_QUANTITY)
};

TASK_RESOURCE_DESC(&idx_resource);
