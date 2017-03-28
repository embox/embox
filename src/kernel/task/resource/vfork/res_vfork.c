/**
 * @file    res_vfork.c
 * @brief   Task resource for vfork
 * @date    May 16, 2014
 * @author  Anton Bondarev
 */

#include <stddef.h>
#include <setjmp.h>

#include <util/binalign.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_vfork.h>


TASK_RESOURCE_DEF(task_vfork_desc, struct task_vfork);

static void task_vfork_init(const struct task *task, void *vfork_buff) {
	task_vfork_end((struct task *)task);
}

static int task_vfork_inherit(const struct task *task,
		const struct task *parent) {
	return 0;
}

static size_t task_vfork_offset;

static const struct task_resource_desc task_vfork_desc = {
	.init = task_vfork_init,
	.inherit = task_vfork_inherit,
	.resource_size = sizeof(struct task_vfork),
	.resource_offset = &task_vfork_offset,
};

struct task_vfork *task_resource_vfork(const struct task *task) {
	size_t offset;
	assert(task != NULL);

	offset = (size_t)((void *)task->resources + task_vfork_offset);
#ifdef PT_REGS_ALIGN
	return (void *)binalign_bound(offset, PT_REGS_ALIGN);
#else
	return (void *)offset;
#endif
}
