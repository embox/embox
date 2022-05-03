/**
 * @file
 * @brief
 *
 * @date 04.03.14
 * @author Ilia Vaprol
 */

#ifndef KERNEL_TASK_MULTI_H_
#define KERNEL_TASK_MULTI_H_

#include <assert.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/thread.h>
#include <kernel/task/task_priority.h>

#define MAX_TASK_NAME_LEN 20

struct thread;

struct task {
	int status;
	int tsk_id;

	struct task *parent;
	struct dlist_head child_list;
	struct dlist_head child_lnk;

	char tsk_name[MAX_TASK_NAME_LEN];
	struct thread *tsk_main;
	task_priority_t tsk_priority;
	clock_t tsk_clock;
	char resources[];
};



__BEGIN_DECLS

#include <kernel/task/defs.h>

static inline int task_is_vforking(struct task *task) {
	return task->status & TASK_STATUS_IN_VFORK;
}

static inline void task_vfork_start(struct task *task) {
	task->status |= TASK_STATUS_IN_VFORK;
}

static inline void task_vfork_end(struct task *task) {
	task->status &= ~TASK_STATUS_IN_VFORK;
}

static inline int task_get_status(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->status;
}

static inline int task_get_id(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->tsk_id;
}

static inline const char * task_get_name(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->tsk_name;
}

static inline int task_set_name(struct task *tsk, const char *name) {
	strncpy(tsk->tsk_name, name, sizeof(tsk->tsk_name) - 1);
	tsk->tsk_name[sizeof(tsk->tsk_name) - 1] = '\0';
	return 0;
}

static inline struct thread * task_get_main(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->tsk_main;
}

static inline struct task * task_get_parent(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->parent;
}

static inline void task_set_main(struct task *tsk,
		struct thread *main_thread) {
	assert(tsk != NULL);
	assert(main_thread != NULL);
	assert(tsk->tsk_main == NULL);

	tsk->tsk_main = main_thread;
	main_thread->task = tsk;
}

static inline void task_thread_register(struct task *tsk, struct thread *t) {
	assert(tsk != NULL);
	assert(t != NULL);
	assert(task_get_main(tsk) != NULL);
	assert(t->task == NULL);

	dlist_add_next(&t->thread_link, &task_get_main(tsk)->thread_link);
	t->task = tsk;
}

static inline void task_thread_unregister(struct task *tsk, struct thread *t) {
	assert(tsk != NULL);
	assert(t != NULL);

	if (t != task_get_main(tsk)) {
		dlist_del(&t->thread_link);
	} else {
		assert(dlist_empty(&task_get_main(tsk)->thread_link));
		tsk->tsk_main = NULL;
	}

	/* XXX t->task isn't set to null, thread allowed to know old parent while
 	 * shutting down
	 */
}

static inline task_priority_t task_get_priority(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->tsk_priority;
}

static inline clock_t task_get_clock(const struct task *tsk) {
	assert(tsk != NULL);
	return tsk->tsk_clock;
}

static inline void task_set_clock(struct task *tsk, clock_t new_clock) {
	assert(tsk != NULL);
	tsk->tsk_clock = new_clock;
}

__END_DECLS

#endif /* KERNEL_TASK_MULTI_H_ */
