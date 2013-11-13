/**
 * @file
 *
 * @date Nov 13, 2013
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <kernel/task.h>

static size_t idesc_table_offset;

#define res_get_pointer(task) ((void*)task + idesc_table_offset)

static void idesc_table_res_init(struct task *task, void *data) {
	struct idesc_table *t;

	t = data;
	index_init(&t->indexator, 0, IDESC_QUANTITY, t->idesc_table);
}

static int idesc_table_inherit(struct task *task, struct task *parent) {
	struct idesc_table *t, *parent_table;

	t = res_get_pointer(task);
	parent_table = res_get_pointer(parent);

	idesc_table_fork(t, parent_table);
	return 0;
}

static void idesc_table_deinit(struct task *task) {

}

static int idesc_table_resource_init(const struct task_resource_desc *desc,
		size_t offset) {
	idesc_table_offset = offset;
	return 0;
}

static const struct task_resource_desc idesc_table_resource = {
	.init = idesc_table_res_init,
	.inherit = idesc_table_inherit,
	.deinit = idesc_table_deinit,
	.resource_size = sizeof(struct idesc_table),
	.resource_init = idesc_table_resource_init
};


TASK_RESOURCE_DESC(&idesc_table_resource);
