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
#include <kernel/task_resources.h>

/**
 * @brief Task describing struct
 */
struct task {
	struct task *parent; /**< @brief Task's parent */

	struct list_head children; /**< @brief Task's children */
	struct list_head link; /**< @brief task's list link (handle task in lists) */

	struct list_head threads; /**< @brief Threads which have task pointer this task */

	struct task_resources resources; /**< @brief Resources which task have */

	int errno; /**< @brief Last occured error code */
};

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
 * @brief Get task resources struct from task
 * @param task Task to get from
 * @return Task resources from task
 */
static inline struct task_resources *task_get_resources(struct task *task) {
	return &task->resources;
}
/**
 * @brief Determ is given fd is valid to use with tasks
 * @param fd File descriptor number to test
 * @return If given fs is valid to use with tasks
 */
static inline int task_valid_fd(int fd) {
	return 0 <= fd && fd <= CONFIG_TASKS_RES_QUANTITY;
}

/**
 * @brief Kernel task
 *
 * @return Pointer to default task
 */
extern struct task *task_default_get(void);

/**
 * @brief Get task resources of self task
 *
 * @return Task resources of self task
 */
static inline struct task_resources *task_self_res(void) {
	return task_get_resources(task_self());
}

static inline struct idx_desc *task_self_idx_get(int fd) {
	assert(task_valid_fd(fd));
	return task_res_idx_get(task_self_res(), fd);
}

/**
 * @brief Set idx descriptor of self task
 *
 * @param fd idx descriptor number
 * @param desc idx descriptor pointer to associate with number
 */
static inline void task_self_idx_set(int fd, struct idx_desc *desc) {
	task_res_idx_set(task_self_res(), fd, desc);
}

extern int task_self_idx_alloc(const struct task_res_ops *res_ops, void *data);

static inline int task_self_idx_first_unbinded(void) {
	return task_res_idx_first_unbinded(task_self_res());
}

static inline int task_valid_binded_fd(int fd) {
	return task_valid_fd(fd) && task_res_idx_is_binded(task_self_res(), fd);
}

static inline int task_valid_unbinded_fd(int fd) {
	return task_valid_fd(fd) && !task_res_idx_is_binded(task_self_res(), fd);
}

#endif /* TASK_H_ */
