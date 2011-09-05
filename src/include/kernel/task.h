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
	FILE *stdin;
	FILE *stdout;
	FILE *stderr;

} task_t;

extern int task_create(struct task **new, struct task *parent);

extern struct task *task_self(void);

extern struct task *task_default_get(void);

#endif /* TASK_H_ */
