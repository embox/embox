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
	struct list_head link;
	FILE *file;
	int fd;
};

#define FD_N_MAX 16

struct __fd_array {
	struct list_head free_fds;
	struct list_head opened_fds;
	struct __fd_list fds[FD_N_MAX];
};

typedef struct task {
	struct task *parent;

	struct list_head children;
	struct list_head link;

	struct list_head threads;

	/* files */
	struct __fd_array fd_array;

} task_t;

extern int task_create(struct task **new, struct task *parent);

extern struct task *task_self(void);

extern struct task *task_default_get(void);

#endif /* TASK_H_ */
