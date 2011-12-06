/**
 * @file
 * @brief
 *
 * @date 16.09.11
 * @author Anton Kozlov
 */
#include <embox/unit.h>
#include <kernel/task.h>
#include <diag/diag_device.h>
#include <mem/objalloc.h>

#include "index_desc.h"

EMBOX_UNIT_INIT(tasks_init);

static struct task default_task;
#if 0
int task_idx_save_res(int fd, void *desc, struct task_resources *res);

int task_desc2idx(struct __fd_list *desc, struct task_resources *res) {
	return ((int) desc - (int) res->fds) / sizeof(struct __fd_list);
}

void* task_idx2desc(int fd, struct task_resources *res) {
	return res->fds[fd].file;
}
void fd_list_init(struct task_resources *res) {
	for (int i = 0; i < CONFIG_TASKS_FILE_QUANTITY; i++) {
		INIT_LIST_HEAD(&res->fds[i].link);
	}
}
#endif

OBJALLOC_DEF(idx_res_pool, struct idx_desc, CONFIG_TASKS_RES_QUANTITY);

void res_init(struct task_resources *res) {
	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		task_res_idx_set(res, i, NULL);
	}
}

struct idx_desc *task_idx_desc_alloc(int type, void *data) {
	struct idx_desc *desc = objalloc(&idx_res_pool);
	desc->link_count = 1; //always assigns returned value

	desc->type = type;
	desc->data = data;
	return desc;
}

void task_idx_desc_free(struct idx_desc *desc) {
	objfree(&idx_res_pool, desc);
}

int task_res_idx_alloc(struct task_resources *res, int type, void *data) {
	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		if (!task_res_idx_is_binded(res, i)) {
			task_res_idx_set(res, i, task_idx_desc_alloc(type, data));
			return i;
		}
	}
	return -1;
}

void task_res_idx_free(struct task_resources *res, int idx) {
	task_idx_desc_free(task_res_idx_get(res, idx));

	task_res_idx_unbind(res, idx);
}

void task_root_init(struct task *new_task) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->children);
	INIT_LIST_HEAD(&new_task->link);

	new_task->parent = NULL;

	res_init(task_get_resources(new_task));
}

struct task *task_default_get(void) {
	return &default_task;
}

static int tasks_init(void) {
	FILE* file = diag_device_get();
	struct task_resources *res = task_get_resources(&default_task);

	task_root_init(&default_task);
	task_res_idx_set(res, 0, task_idx_desc_alloc(TASK_IDX_TYPE_FILE, file));
	task_res_idx_set(res, 1, task_idx_desc_alloc(TASK_IDX_TYPE_FILE, file));
	task_res_idx_set(res, 2, task_idx_desc_alloc(TASK_IDX_TYPE_FILE, file));

	return 0;
}

