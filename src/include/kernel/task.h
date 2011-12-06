/**
 * @file
 * @brief
 *
 * @date 17.04.11
 * @author Anton Bondarev
 */

#ifndef TASK_H_
#define TASK_H_

#include <fs/file.h>
#include <lib/list.h>
#include <kernel/task_resources.h>

struct task {
	struct task *parent;

	struct list_head children;
	struct list_head link;

	struct list_head threads;

	struct task_resources resources;

	int errno;
};

extern int task_create(struct task **new, struct task *parent);

extern struct task *task_self(void);

extern int task_delete(struct task *tsk);


static inline struct task_resources *task_get_resources(struct task *task) {
	return &task->resources;
}
//TODO
static inline int task_valid_fd(int fd) {
	return 0 <= fd && fd <= CONFIG_TASKS_RES_QUANTITY;
}


//TODO
extern struct task *task_default_get(void);

//TODO
static inline struct task_resources *task_self_res(void) {
	return task_get_resources(task_self());
}
//TODO
static inline struct idx_desc *task_self_idx_get(int fd) {
	return task_res_idx_get(task_self_res(), fd);
}

//TODO
static inline void task_self_idx_set(int fd, struct idx_desc *desc) {
	task_res_idx_set(task_self_res(), fd, desc);
}

#endif /* TASK_H_ */
