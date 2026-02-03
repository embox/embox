/**
 * @file
 *
 * @date Nov 13, 2013
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/idesc_table.h>

TASK_RESOURCE_DEF(task_idesc_table_desc, struct idesc_table);

static void task_idesc_table_init(const struct task *task,
		void *idesc_table_space) {
	struct idesc_table *it;

	assert(idesc_table_space == task_resource_idesc_table(task));

	it = idesc_table_space;
	assert(it != NULL);

	idesc_table_init(it);
}

static int task_idesc_table_inherit(const struct task *task,
		const struct task *parent) {
	struct idesc_table *it, *parent_it;

	it = task_resource_idesc_table(task);
	assert(it != NULL);

	parent_it = task_resource_idesc_table(parent);
	assert(parent_it != NULL);

	return idesc_table_fork(it, parent_it);
}

static void task_idesc_table_deinit(const struct task *task) {
	struct idesc_table *it;

	it = task_resource_idesc_table(task);
	assert(it != NULL);

	idesc_table_finit(it);
}

static size_t task_idesc_table_offset;

static const struct task_resource_desc task_idesc_table_desc = {
	.init = task_idesc_table_init,
	.inherit = task_idesc_table_inherit,
	.deinit = task_idesc_table_deinit,
	.resource_size = sizeof(struct idesc_table),
	.resource_offset = &task_idesc_table_offset
};

struct idesc_table * task_resource_idesc_table(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_idesc_table_offset;
}
