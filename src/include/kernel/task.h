/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <lib/list.h>
#include <kernel/task/idx.h>

/**
 * @brief Task resources container
 */
/**
 * @brief Task describing struct
 */
struct task {
	struct task *parent; /**< @brief Task's parent */

	struct list_head children; /**< @brief Task's children */
	struct list_head link; /**< @brief task's list link (handle task in lists) */

	struct list_head threads; /**< @brief Threads which have task pointer this task */

	struct task_idx_table idx_table; /**< @brief Resources which task have */

	int errno; /**< @brief Last occured error code */
};

/**
 * @brief Get task resources struct from task
 * @param task Task to get from
 * @return Task resources from task
 */
static inline struct task_idx_table *task_idx_table(struct task *task) {
	return &task->idx_table;
}

/**
 * @brief Create new task with resources inhereted from parent task
 *
 * @param new Pointer where new task's pointer will be stored
 * @param parent Pointer to a parent task
 *
 * @return 0 for success, negative on error occur
 */
extern int task_create(struct task **new, struct task *parent);

/**
 * @brief Get self task (task which current execution thread associated with)
 *
 * @return Pointer to self task
 */
extern struct task *task_self(void);

/**
 * @brief TODO
 *
 * @param tsk
 *
 * @return
 */
extern int task_delete(struct task *tsk);

/**
 * @brief Kernel task
 *
 * @return Pointer to kernel task
 */
extern struct task *task_kernel_task(void);

#endif /* TASK_H_ */
