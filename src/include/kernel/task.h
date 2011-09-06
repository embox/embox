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

struct __fd_list {
	struct list_head list_hnd;
	FILE *file;
	int fd;
};

typedef struct task {
	struct task *parent;

	struct list_head child_tasks;
	struct list_head child_link;

	struct list_head threads;

	struct list_head free_fds;
	struct list_head opened_fds;
	struct __fd_list fds[16];

} task_t;

extern int task_create(struct task **new, struct task *parent);

extern struct task *task_self(void);

extern struct task *task_default_get(void);

#endif /* TASK_H_ */
