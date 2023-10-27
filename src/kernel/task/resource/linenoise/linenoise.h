/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 27.10.23
 */
#ifndef KERNEL_TASK_RESOURCE_LINENOISE_LINENOISE_H_
#define KERNEL_TASK_RESOURCE_LINENOISE_LINENOISE_H_

#include <termios.h>

#include <kernel/task.h>

struct task_linenoise {
	struct termios orig_termios;
	int maskmode;
	int rawmode;
	int mlmode;
	int atexit_registered;
	int history_max_len;
	int history_len;
	char **history;
};

__BEGIN_DECLS

extern struct task_linenoise *task_resource_linenoise(const struct task *task);

__END_DECLS

#define task_self_resource_linenoise() task_resource_linenoise(task_self())

#endif /* KERNEL_TASK_RESOURCE_LINENOISE_LINENOISE_H_ */
