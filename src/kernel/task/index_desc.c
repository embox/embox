/**
 * @file
 * @brief
 *
 * @date 16.09.11
 * @author Anton Kozlov
 */
#include <embox/unit.h>
#include <kernel/task.h>
#include <mem/objalloc.h>

#include "index_desc.h"


OBJALLOC_DEF(idx_res_pool, struct idx_desc, CONFIG_TASKS_RES_QUANTITY);

void task_idx_table_init(struct task_idx_table *res) {
	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		task_idx_table_set(res, i, NULL);
	}
}

/*static*/ struct idx_desc *task_idx_desc_alloc(const struct task_idx_ops *res_ops, void *data) {
	struct idx_desc *desc = objalloc(&idx_res_pool);
	desc->link_count = 0;

	desc->res_ops = res_ops;
	desc->data = data;
	return desc;
}

static void task_idx_desc_free(struct idx_desc *desc) {
	objfree(&idx_res_pool, desc);
}

int task_idx_table_first_unbinded(struct task_idx_table *res) {
	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		if (!task_idx_table_is_binded(res, i)) {
			return i;
		}
	}
	return -1;
}

int task_idx_table_set(struct task_idx_table *res, int idx, struct idx_desc *desc) {
	struct idx_desc *old_idx = task_idx_table_get(res, idx);
	int ret = 0;
	assert(res);
	if (old_idx) {
		if (0 == task_idx_desc_link_count_add(old_idx, -1)) {
			ret = task_idx_desc_ops(old_idx)->close(old_idx->data);
			task_idx_desc_free(old_idx);
		}
	}

	res->idx[idx] = desc;

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

