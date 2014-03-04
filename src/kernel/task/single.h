/**
 * @file
 * @brief
 *
 * @date 04.03.14
 * @author Ilia Vaprol
 */

#ifndef KERNEL_TASK_SINGLE_H_
#define KERNEL_TASK_SINGLE_H_

#include <assert.h>
#include <errno.h>
#include <kernel/task/task_priority.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

struct task {
	char resources[];
};

__BEGIN_DECLS

static inline int task_get_id(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return 0;
}

static inline const char * task_get_name(
		const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return "kernel";
}

static inline struct thread * task_get_main(
		const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return cpu_idle(cpu_get_id());
}

static inline task_priority_t task_get_priority(
		const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return 0;
}

static inline int task_set_priority(struct task *tsk,
		task_priority_t new_priority) {
	assert(tsk == task_kernel_task());
	return -ENOSYS;
}

static inline clock_t task_get_clock(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return cpu_get_total_time(cpu_get_id());
}

static inline void task_set_clock(struct task *tsk,
		clock_t new_clock) {
	assert(tsk == task_kernel_task());
}

static inline struct task * task_self(void) {
	/* Since there is only one task, actually it means --
	 * that task is kernel's.
	 */
	return task_kernel_task();
}

static inline int new_task(const char *name, void *(*run)(void *),
		void *arg) {
	return -EPERM;
}

static inline void task_init(struct task *tsk, int id,
		const char *name, struct thread *main_thread,
		task_priority_t priority) {
	assert(tsk == task_kernel_task());
}

static inline void __attribute__((noreturn)) task_exit(void *res) {
	while(1);
}

static inline int task_notify_switch(struct thread *prev,
		struct thread *next) {
	return 0;
}

static inline int task_waitpid(pid_t pid) {
	return -ECHILD;
}

__END_DECLS

#endif /* KERNEL_TASK_SINGLE_H_ */
