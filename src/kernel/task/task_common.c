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

void fd_list_init(struct __fd_array *fd_array) {
	INIT_LIST_HEAD(&fd_array->free_fds);
	INIT_LIST_HEAD(&fd_array->opened_fds);

	for (int i = 0; i < FD_N_MAX; i++) {
		INIT_LIST_HEAD(&fd_array->fds[i].link);
		list_add_tail(&fd_array->fds[i].link,
						&fd_array->free_fds);
	}
}

void task_root_init(struct task *new_task) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->children);
	INIT_LIST_HEAD(&new_task->link);

	new_task->parent = NULL;

	fd_list_init(&new_task->fd_array);
}

struct task *task_default_get(void) {
	return &default_task;
}

static int tasks_init(void) {
	FILE* file = diag_device_get();

	task_root_init(&default_task);

	__file_opened_fd(0, file, &default_task);
	__file_opened_fd(1, file, &default_task);
	__file_opened_fd(2, file, &default_task);

	return 0;
}

int task_idx_alloc(int type) {
	struct task *task = task_self();
	switch(type) {
	case TASK_IDX_TYPE_FILE:
		return task->rc_manager.file_idx_cnt++;
	case TASK_IDX_TYPE_SOCKET:
		return task->rc_manager.sock_idx_cnt++;
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
		//task->fd_array.socket_fds[fd & 0xFF] = data;
		return 0;
	case TASK_IDX_TYPE_SOCKET:
		task->fd_array.socket_fds[fd & 0xFF].socket = (struct socket *)desc;
		return 0;
	default:
		return -1;
	}
}

void * task_idx_to_desc(int fd){
	struct task *task = task_self();
	switch(task_idx_to_type(fd)) {
	case TASK_IDX_TYPE_FILE:
		//task->fd_array.socket_fds[fd & 0xFF] = data;
		return NULL;
	case TASK_IDX_TYPE_SOCKET:
		return (void *)task->fd_array.socket_fds[fd & 0xFF].socket;

	default:
		return NULL;
	}
}

int task_idx_release(int idx) {
	return task_idx_save(idx, NULL);
}
