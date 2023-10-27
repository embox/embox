/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 27.10.23
 */
#include <assert.h>
#include <limits.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <lib/linenoise.h>

#include "linenoise.h"

TASK_RESOURCE_DEF(task_linenoise_desc, struct task_linenoise);

static void task_linenoise_init(const struct task *task,
    void *linenoise_space) {
	struct task_linenoise *task_linenoise;

	assert(linenoise_space == task_resource_linenoise(task));

	task_linenoise = linenoise_space;

	task_linenoise->maskmode = 0;
	task_linenoise->rawmode = 0;
	task_linenoise->mlmode = 0;
	task_linenoise->atexit_registered = 0;
	task_linenoise->history_max_len = LINENOISE_DEFAULT_HISTORY_MAX_LEN;
	task_linenoise->history_len = 0;
	task_linenoise->history = NULL;
}

static size_t task_linenoise_offset;

static const struct task_resource_desc task_linenoise_desc = {
    .init = task_linenoise_init,
    .resource_size = sizeof(struct task_linenoise),
    .resource_offset = &task_linenoise_offset};

struct task_linenoise *task_resource_linenoise(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_linenoise_offset;
}
