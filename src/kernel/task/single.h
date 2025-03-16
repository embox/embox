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
#include <compiler.h>
#include <errno.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <time.h>

#include <framework/mod/options.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/task_priority.h>
#include <kernel/thread.h>

struct task {
	struct thread *tsk_main;
	char resources[];
};

__BEGIN_DECLS

static inline int task_is_vforking(struct task *task) {
	(void)task;

	return 0;
}

static inline void task_vfork_start(struct task *task) {
	(void)task;
}

static inline void task_vfork_end(struct task *task) {
	(void)task;
}

static inline int task_get_status(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return 0;
}

static inline int task_get_id(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return 0;
}

static inline const char *task_get_name(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return "kernel";
}

static inline struct thread *task_get_main(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return tsk->tsk_main;
}

static inline struct task *task_get_parent(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return NULL;
}

static inline task_priority_t task_get_priority(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return TASK_PRIORITY_DEFAULT;
}

static inline int task_set_priority(struct task *tsk,
    task_priority_t new_priority) {
	assert(tsk == task_kernel_task());
	return -ENOSYS;
}

static inline clock_t task_get_clock(const struct task *tsk) {
	assert(tsk == task_kernel_task());
	return clock();
}

static inline void task_set_clock(struct task *tsk, clock_t new_clock) {
	assert(tsk == task_kernel_task());
}

static inline struct task *task_self(void) {
	/* Since there is only one task, actually it means --
	 * that task is kernel's.
	 */
	return task_kernel_task();
}

static inline int new_task(const char *name, void *(*run)(void *), void *arg) {
	return -EPERM;
}

static inline void _NORETURN task_exit(void *res) {
	while (1) {}
}

static inline void task_thread_register(struct task *tsk, struct thread *t) {
	assert(tsk != NULL);
	assert(t != NULL);
	assert(task_get_main(tsk) != NULL);
	assert(t->task == NULL);

	t->task = tsk;
}

static inline void task_thread_unregister(struct task *tsk, struct thread *t) {
	assert(tsk != NULL);
	assert(t != NULL);

	/* XXX t->task isn't set to null, thread allowed to know old parent while
 	 * shutting down
	 */
}

static inline rlim_t task_getrlim_stack_size(struct task *tsk) {
	return THREAD_DEFAULT_STACK_SIZE;
}

static inline void task_setrlim_stack_size(struct task *tsk, rlim_t stack_sz) {
	/* do nothing */
}

#include <kernel/thread/types.h>
static inline void task_set_main(struct task *tsk, struct thread *main_thread) {
	assert(tsk == task_kernel_task());
	assert(main_thread != NULL);
	tsk->tsk_main = main_thread;
	main_thread->task = tsk;
}

__END_DECLS

#endif /* KERNEL_TASK_SINGLE_H_ */
