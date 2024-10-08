/**
 * @file
 * @brief
 *
 * @date 11.03.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <sys/resource.h>
#include <sys/types.h>

#include <kernel/task.h>
#include <kernel/task/resource/u_area.h>
#include <kernel/task/task_table.h>
#include <util/math.h>

static int xetpriority_match(int which, id_t who, struct task *task) {
	struct task_u_area *task_u_area;
	struct task_u_area *self_u;
	struct task *self;
	id_t real_who;
	int is_match = 0; /* "may be used uninitialized" error with NDEBUG macro */

	if (who != 0) {
		real_who = who;
	}
	else {
		self = task_self();
		self_u = task_resource_u_area(self);

		switch (which) {
		case PRIO_PROCESS:
			real_who = task_get_id(task_self());
			break;
		case PRIO_PGRP:
			real_who = self_u->regid;
			break;
		case PRIO_USER:
			real_who = self_u->reuid;
			break;
		default:
			assert(0);
		}
	}

	task_u_area = task_resource_u_area(task);

	switch (which) {
	case PRIO_PROCESS:
		is_match = task_get_id(task) == real_who;
		break;
	case PRIO_PGRP:
		is_match = task_u_area->regid == real_who;
		break;
	case PRIO_USER:
		is_match = task_u_area->reuid == real_who;
		break;
	default:
		assert(0);
	}

	return is_match;
}

static int xetpriority_which_ok(int which) {
	return (which == PRIO_PROCESS) || (which == PRIO_PGRP)
	       || (which == PRIO_USER);
}

int getpriority(int which, id_t who) {
	struct task *task;
	int priority;

	if (!xetpriority_which_ok(which)) {
		return SET_ERRNO(EINVAL);
	}

	priority = INT_MAX;
	task_foreach(task) {
		if (xetpriority_match(which, who, task)) {
			priority = min(priority, task_get_priority(task));
		}
	}

	return priority != INT_MAX ? priority : SET_ERRNO(ESRCH);
}

int setpriority(int which, id_t who, int value) {
	int ret, exist;
	struct task *task;

	if (!xetpriority_which_ok(which)) {
		return SET_ERRNO(EINVAL);
	}

	exist = 0;
	task_foreach(task) {
		if (xetpriority_match(which, who, task)) {
			exist = 1;

			ret = task_set_priority(task, value);
			if (ret != 0) {
				return SET_ERRNO(-ret);
			}
		}
	}

	return exist ? 0 : SET_ERRNO(ESRCH);
}
