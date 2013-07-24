/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <util/dlist.h>
#include <util/array.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#include <kernel/task/task_priority.h>

#define MAX_TASK_NAME_LEN 20

#define TASK_AFFINITY_DEFAULT ((unsigned int)-1)

__BEGIN_DECLS

struct task_signal_table;
struct task_idx_table;
struct thread;
struct emmap;
struct task_u_area;
struct task_env;
struct sleepq;


/**
 * @brief Task structure description
 */
struct task {
	struct dlist_head task_link; /**< @brief global task's list link */

	int tid;               /**< task identifier */

	char task_name[MAX_TASK_NAME_LEN]; /**< @brief Task's name */

	struct task *parent; /**< @brief Task's parent */

	struct dlist_head children_tasks; /**< @brief Task's children */

	struct thread *main_thread;

	struct task_idx_table *idx_table; /**< @brief Resources which task have */

	struct task_signal_table *signal_table;

	struct emmap *mmap;



	struct task_u_area *u_area;

	struct task_env *env;

	task_priority_t priority; /**< @brief Task priority */

	void   *security;

	int err; /**< @brief Last occurred error code */

	clock_t per_cpu; /**< task times */

	struct wait_queue *waitq;

	unsigned int affinity;
};

struct task_resource_desc {
	void (*init)(struct task *task, void *resource_space);
	int (*inherit)(struct task *task, struct task *parent_task);
	void (*deinit)(struct task *task);
	size_t resource_size; /* to be used in on-stack allocation */
};

typedef int (*task_notifing_resource_hnd)(struct thread *prev, struct thread *next);

extern const struct task_resource_desc *task_resource_desc_array[];

extern const task_notifing_resource_hnd task_notifing_resource[];

#define TASK_RESOURCE_DESC(res) \
	ARRAY_SPREAD_ADD(task_resource_desc_array, res)

#define TASK_RESOURCE_NOTIFY(fn) \
	ARRAY_SPREAD_ADD(task_notifing_resource, fn)

/**
 * @brief Get task resources structure from task
 *
 * @param task Task to get from
 * @return Task resources from task
 */
static inline struct task_idx_table *task_idx_table(struct task *task) {
	return task->idx_table;
}

/** create new task and initialize its descriptor */
extern int new_task(const char *name, void *(*run)(void *), void *arg);

/** insert a created task into the task */
extern int task_add_thread(struct task *, struct thread *);

extern int task_remove_thread(struct task *, struct thread *);

/**
 * @brief Get self task (task which current execution thread associated with)
 *
 * @return Pointer to self task
 */
extern struct task *task_self(void);

/** return ID of a current task */
static inline int task_getid(void) {
	return task_self()->tid;
}

/** setup task priority */
extern int task_set_priority(struct task *task, task_priority_t priority);

/** get task priority */
extern task_priority_t task_get_priority(struct task *task);

/* this is for SMP mode */
static inline void task_set_affinity(struct task *task, unsigned int affinity) {
	task->affinity = affinity;
}
static inline unsigned int task_get_affinity(struct task *task) {
	return task->affinity;
}

static inline void *task_self_security(void) {
	return task_self()->security;
}

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

__END_DECLS

#define task_foreach_thread(thr, task)                                    \
	thr = task->main_thread;                                             \
	for (struct thread *nxt = dlist_entry(thr->thread_task_link.next,    \
			struct thread, thread_task_link);                            \
		thr != task->main_thread;                                        \
		thr = nxt,                                                       \
			nxt = dlist_entry(thr->thread_task_link.next,                \
						struct thread, thread_task_link)                 \
		)

#define task_foreach(task)                             \
	task = task_table_get(0);                          \
	for(int tid = 1;                                   \
	(task != NULL);                                    \
	task = task_table_get(task_table_get_first(tid++)) \
	)

#endif /* TASK_H_ */
