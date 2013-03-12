/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <sys/resource.h>
#include <kernel/task.h>
#include <errno.h>
#include <kernel/task/task_table.h>
#include <kernel/task/u_area.h>

int getpriority(int which, id_t who) {
	int tid;
	struct task *task;

	for (tid = 0; (tid = task_table_get_first(tid)) >= 0; ++tid) {
		task = task_table_get(tid);
		switch (which) {
		default:
			SET_ERRNO(EINVAL);
			return -1;
		case PRIO_PROCESS:
			if (task->tid == who) {
				return task_get_priority(task);
			}
			break;
		case PRIO_PGRP:
			if (task->u_area->regid == who) {
				return task_get_priority(task);
			}
			break;
		case PRIO_USER:
			if (task->u_area->reuid == who) {
				return task_get_priority(task);
			}
			break;
		}
	}

	SET_ERRNO(-tid);
	return -1;
}

int setpriority(int which, id_t who, int value) {
	int tid, ret, exist;
	struct task *task;

	exist = 0;
	for (tid = 0; (tid = task_table_get_first(tid)) >= 0; ++tid) {
		task = task_table_get(tid);
		switch (which) {
		default:
			SET_ERRNO(EINVAL);
			return -1;
		case PRIO_PROCESS:
			if (task->tid == who) {
				ret = task_set_priority(task, value);
				if (ret != 0) {
					SET_ERRNO(-ret);
					return -1;
				}
				exist = 1;
			}
			break;
		case PRIO_PGRP:
			if (task->u_area->regid == who) {
				ret = task_set_priority(task, value);
				if (ret != 0) {
					SET_ERRNO(-ret);
					return -1;
				}
				exist = 1;
			}
			break;
		case PRIO_USER:
			if (task->u_area->reuid == who) {
				ret = task_set_priority(task, value);
				if (ret != 0) {
					SET_ERRNO(-ret);
					return -1;
				}
				exist = 1;
			}
			break;
		}
	}

	if (!exist) {
		SET_ERRNO(-tid);
		return -1;
	}

	return 0;
}
