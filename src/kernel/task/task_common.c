/**
 * @file
 * @brief
 *
 * @date 16.09.11
 * @author Anton Kozlov
 */

#include <kernel/task.h>
#include "task_common.h"
#include <diag/diag_device.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(tasks_init);

static struct task default_task;

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

void task_root_init(struct task *new_task) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->children);
	INIT_LIST_HEAD(&new_task->link);

	new_task->parent = NULL;

	fd_list_init(&new_task->resources);
}

struct task *task_default_get(void) {
	return &default_task;
}

static int tasks_init(void) {
	FILE* file = diag_device_get();
	struct task_resources *res = &default_task.resources;

	task_root_init(&default_task);
	task_idx_save_res(0, file, res);
	task_idx_save_res(1, file, res);
	task_idx_save_res(2, file, res);

	return 0;
}

int task_idx_alloc_res(int type, struct task_resources *res) {
	for (int i = 0; i < CONFIG_TASKS_FILE_QUANTITY; i++) {
		if (res->fds[i].file == NULL) {
			res->fds[i].file = (void *) 1;
			res->fds[i].type = type;
			return i;
		}
	}

	return -1;
}

int task_idx_free_res(int idx, struct task_resources *res) {
	res->fds[idx].file = NULL;
	return 0;
}

int task_idx_alloc(int type) {
	//struct task_resources *res = &task_self()->resources;
	//return task_desc2idx(task_fdl_alloc(res), res);

	return task_idx_alloc_res(type, task_get_resources(task_self()));

}


int task_idx_save_res(int fd, void *desc, struct task_resources *res) {
	res->fds[fd].file = desc;
	return 0;
}

void *task_idx_get_res(int fd, struct task_resources *res) {
	return res->fds[fd].file;
}

int task_idx_to_type(int fd) {
	struct task_resources *res = task_get_resources(task_self());
	return res->fds[fd].type;
}

int task_idx_save(int fd, void *desc) {
	return task_idx_save_res(fd, desc, task_get_resources(task_self()));
}

void * task_idx_to_desc(int fd){
	return task_idx2desc(fd, task_get_resources(task_self()));
}

int task_idx_release(int idx) {
	task_idx_free_res(idx, task_get_resources(task_self()));
	return 0;
}
