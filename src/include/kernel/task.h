/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <assert.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/task/task_priority.h>

#define MAX_TASK_NAME_LEN 20

__BEGIN_DECLS

struct thread;


/**
 * @brief Task structure description
 */
struct task {
	/* multi */
	task_priority_t tsk_priority; /**< @brief Task priority */

	/* common */
	int tsk_id;               /**< task identifier */
	char tsk_name[MAX_TASK_NAME_LEN]; /**< @brief Task's name */
	struct thread *tsk_main;
	clock_t tsk_clock; /**< task times */

	char resources[];
};


/** create new task and initialize its descriptor */
extern int new_task(const char *name, void *(*run)(void *), void *arg);

/**
 * @brief Get self task (task which current execution thread associated with)
 *
 * @return Pointer to self task
 */
extern struct task *task_self(void);

static inline int task_get_id(const struct task *tsk) {
	return task_self()->tsk_id;
}

static inline const char * task_get_name(
		const struct task *tsk) {
	return tsk->tsk_name;
}

/** setup task priority */
extern int task_set_priority(struct task *task, task_priority_t priority);

static inline task_priority_t task_get_priority(
		const struct task *tsk) {
	return tsk->tsk_priority;
}

static inline struct thread * task_get_main(
		const struct task *tsk) {
	return tsk->tsk_main;
}

static inline clock_t task_get_clock(const struct task *tsk) {
	return tsk->tsk_clock;
}

static inline void task_set_clock(struct task *tsk,
		clock_t new_clock) {
	tsk->tsk_clock = new_clock;
}

extern struct task * task_init(void *space, size_t size,
		const char *name);

/**
 * @brief Exit from current task
 *
 * @param res Return code
 */
extern void __attribute__((noreturn)) task_exit(void *res);

/**
 * @brief Kernel task
 *
 * @return Pointer to kernel task
 */
extern struct task *task_kernel_task(void);

extern int task_notify_switch(struct thread *prev, struct thread *next);

extern int task_waitpid(pid_t pid);

#include <util/dlist.h>

#define task_foreach_thread(th, tsk)                                    \
	th = task_get_main(tsk);                                             \
	for (struct thread *nxt = dlist_entry(th->thread_link.next, \
			struct thread, thread_link), \
			*tmp = NULL;                            \
		(th != task_get_main(tsk)) || (tmp == NULL);                      \
		tmp = th, \
		th = nxt,                                                       \
			nxt = dlist_entry(th->thread_link.next,                \
						struct thread, thread_link)                 \
		)

#include <kernel/task/task_table.h>

#define task_foreach(tsk)                             \
	tsk = task_table_get(0);                          \
	for(int tid = 1;                                   \
	(tsk != NULL);                                    \
	tsk = task_table_get(task_table_get_first(tid++)) \
	)

__END_DECLS

#endif /* TASK_H_ */
