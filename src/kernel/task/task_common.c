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

int desc2idx(struct __fd_list *desc, struct task_resources *res) {
	return ((int) desc - (int) res->fds) / sizeof(struct __fd_list);

#if 0
	int i;
	for(i = 0; i < ARRAY_SIZE(parent->resources.fds); i ++) {
		if(parent->resources.fds[i].file == desc->file) {
			return i;
		}
	}
	return -1;
#endif
}

void fd_list_init(struct task_resources *res) {
	INIT_LIST_HEAD(&res->fds_free);

	for (int i = 0; i < CONFIG_TASKS_FILE_QUANTITY; i++) {
		INIT_LIST_HEAD(&res->fds[i].link);
	}
}

struct __fd_list *task_fdl_alloc(struct task_resources *res) {
	struct list_head *next = NULL;
	if (!list_empty(&res->fds_free)) {
		next = (res->fds_free.next);
		list_del_init(next);
	}
	return (struct __fd_list *) next;
}

int task_fdl_free(struct __fd_list *fdl, struct task_resources *res) {
	int fd = desc2idx(fdl, res);
	if (task_valid_fd(fd)) {
		list_add_tail(&fdl->link, &res->fds_free);
		return 0;
	}
	return -1;
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

	task_root_init(&default_task);

	__file_opened_fd(0, file, &default_task.resources);
	__file_opened_fd(1, file, &default_task.resources);
	__file_opened_fd(2, file, &default_task.resources);

	return 0;
}

int task_idx_alloc(int type) {
	struct task *task = task_self();
	switch(type) {
	case TASK_IDX_TYPE_FILE:
		return task->resources.file_idx_cnt++;
	case TASK_IDX_TYPE_SOCKET:
		return (TASK_IDX_TYPE_SOCKET << 8) | task->resources.socket_idx_cnt++;
	default:
		return -1;
	}
}

int task_idx_to_type(int fd) {
	return 0xFF & (fd >> 8);
}

int task_idx_save(int fd, void *desc) {
	struct task *task = task_self();
	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		//task->resources.socket_fds[fd & 0xFF] = data;
		return 0;
	case TASK_IDX_TYPE_SOCKET:
		task->resources.socket_fds[fd & 0xFF].socket = (struct socket *)desc;
		return 0;
	default:
		return -1;
	}
}

void * task_idx_to_desc(int fd){
	struct task *task = task_self();
	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		//task->resources.socket_fds[fd & 0xFF] = data;
		return NULL;
	case TASK_IDX_TYPE_SOCKET:
		return (void *)task->resources.socket_fds[fd & 0xFF].socket;

	default:
		return NULL;
	}
}

int task_idx_release(int idx) {
	return task_idx_save(idx, NULL);
}
