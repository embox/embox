/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    31.05.12
 */

#include <kernel/task.h>
#include <unistd.h>

int isatty(int fd) {
	struct idx_desc *desc = task_self_idx_get(fd);
	return task_idx_desc_ops(desc)->type == TASK_RES_OPS_TTY;
}

