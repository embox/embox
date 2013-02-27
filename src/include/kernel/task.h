/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <linux/list.h>
#include <util/array.h>

#include <sys/cdefs.h>
#include <sys/types.h>

#define MAX_TASK_NAME_LEN 20

__BEGIN_DECLS

struct task_signal_table;
struct task_idx_table;
struct thread;
struct mmap;
struct task_u_area;

/**
 * @brief Task resources container
 */
/**
 * @brief Task describing struct
 */
struct task {
	int tid;
	char name[MAX_TASK_NAME_LEN];
	struct task *parent; /**< @brief Task's parent */

	struct list_head children; /**< @brief Task's children */
	struct list_head link; /**< @brief task's list link (handle task in lists) */

	struct list_head threads; /**< @brief Threads which have task pointer this task */

	struct task_idx_table *idx_table; /**< @brief Resources which task have */

	struct task_signal_table *signal_table;

	struct mmap *mmap;

	struct thread *main_thread;

	struct task_u_area *u_area;

	void   *security;

	int err; /**< @brief Last occurred error code */

	clock_t per_cpu; /**< task times */
};

struct task_resource_desc {
	void (*init)(struct task *task, void *resource_space);
	void (*inherit)(struct task *task, struct task *parent_task);
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
 * @brief Get task resources struct from task
 * @param task Task to get from
 * @return Task resources from task
 */
static inline struct task_idx_table *task_idx_table(struct task *task) {
	return task->idx_table;
}

extern int new_task(void *(*run)(void *), void *arg);

/**
 * @brief Get self task (task which current execution thread associated with)
 *
 * @return Pointer to self task
 */
extern struct task *task_self(void);

static inline int task_getid(void) {
	return task_self()->tid;
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

__END_DECLS

#endif /* TASK_H_ */
