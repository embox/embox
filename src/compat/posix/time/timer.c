/**
 * @file
 *
 * @brief POSIX per-process timers (REALTIME) 
 *
 * @date 24.04.25
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stddef.h>
#include <signal.h>
#include <time.h>

#include <kernel/time/sys_timer.h>

#include <kernel/task.h>

#include <module/embox/kernel/task/resource/timer_table.h>

int timer_create(clockid_t clockid, struct sigevent *evp, timer_t *timerid) {
	struct task_resource_timer_desc desc = {0};
	int idx;
	struct sys_timer *sys_timer;

	if ((timerid == NULL) || (evp == NULL)) {
		return -EINVAL;
	}

	idx = task_resource_timer_table_alloc_index(task_self());
	if (idx < 0) {
		return idx;
	}

	sys_timer = sys_timer_alloc();
	if (NULL == sys_timer) {
		return -ENOMEM;
	}

	desc.timerid = idx;
	desc.sigevent = *evp;
	desc.sys_timer = sys_timer;
	desc.task = task_self();

	task_resource_timer_table_add(task_self(), idx, &desc);

	*timerid = desc.timerid;

	return 0;
}

static void timer_handler(struct sys_timer *timer, void *param) {
	struct task_resource_timer_desc *desc;
	struct task *task;

	desc = param;
	task = desc->task;

	kill(task_get_id(task), desc->sigevent.sigev_signo);

}

int timer_settime(timer_t timerid, int flags, const struct itimerspec *value,
    				struct itimerspec *ovalue) {
	struct task_resource_timer_desc *desc;
	int ms_value;

	desc = task_resource_timer_table_get(task_self(), timerid);
	if (desc == NULL) {
		return -EINVAL;
	}

	if (ovalue != NULL) {
		log_error("timer_settime: ovalue has not been implemented yet");
		return -EINVAL;
	}

	if (value == NULL) {
		log_error("timer_settime: value must be setup now");
		return -EINVAL;
	}

	desc->value = *value;
	ms_value = timespec_to_ms(&value->it_interval);
	sys_timer_set(&desc->sys_timer, SYS_TIMER_PERIODIC, ms_value, timer_handler, desc);

	return 0;
}

int timer_gettime(timer_t timerid, struct itimerspec *value) {
	log_debug("timer_gettime() has not been implemented yet");
	return 0;
}

int timer_getoverrun(timer_t timerid) {
	log_debug("timer_getoverrun() has not been implemented yet");
	return 0;
}

int timer_delete(timer_t timerid) {
	struct task_resource_timer_desc *desc;

	desc = task_resource_timer_table_get(task_self(), timerid);
	if (desc == NULL) {
		return -EINVAL;
	}

	sys_timer_close(desc->sys_timer);

	task_resource_timer_table_del(task_self(), timerid);

	task_resource_timer_table_free_index(task_self(), timerid);

	return 0;
}