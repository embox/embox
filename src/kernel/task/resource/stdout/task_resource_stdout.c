/**
 * @file
 *
 * @date Feb 3, 2021
 * @author Anton Bondarev
 */

#include <stddef.h>

#include <unistd.h>
#include <stdio/internal/file_struct.h>

#include <stdio.h>
#include <fcntl.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>

#include <module/embox/kernel/task/resource/stdout.h>

struct task_resource_sdtout {
/* MUST BE FIRST */
	FILE tr_sdtout_file;
	char tr_stdout_buf[16];
};

TASK_RESOURCE_DEF(task_resource_sdtout_desc, struct task_resource_sdtout);

static void task_resource_sdtout_init(const struct task *task, void *buf) {
	struct task_resource_sdtout *tr_stdout = buf;
	FILE *file;

	file =  &tr_stdout->tr_sdtout_file;

	memset(file, 0, sizeof(FILE));
	file->fd = STDOUT_FILENO;
	file->flags = O_WRONLY;
	file->buftype = _IOLBF;
	file->obuf = tr_stdout->tr_stdout_buf;
	file->obuf_sz = sizeof(tr_stdout->tr_stdout_buf);
}

static int task_resource_sdtout_inherit(const struct task *task,
		const struct task *parent) {
	return 0;
}

static size_t task_resource_sdtout_offset;

static const struct task_resource_desc task_resource_sdtout_desc = {
	.init = task_resource_sdtout_init,
	.inherit = task_resource_sdtout_inherit,
	.resource_size = sizeof(struct task_resource_sdtout),
	.resource_offset = &task_resource_sdtout_offset,
};

FILE *task_resource_sdtout(const struct task *task) {
	size_t offset;
	assert(task != NULL);

	offset = (size_t)((void *)task->resources + task_resource_sdtout_offset);

	return (void *)offset;
}

FILE *task_self_resource_sdtout(void) {
	return task_resource_sdtout(task_self());
}
