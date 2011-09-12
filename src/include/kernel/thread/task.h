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

typedef struct task {
	struct task *parent;

	struct list_head child_tasks;
	struct list_head child_link;

	struct list_head threads;

	struct list_head fd_list;

} task_t;

extern struct task *task_self(void);

#endif /* TASK_H_ */
