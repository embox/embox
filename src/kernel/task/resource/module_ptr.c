/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    28.05.2014
 */

#include <stddef.h>

#include <kernel/task/resource.h>
#include <kernel/task/resource/module_ptr.h>

struct mod;

static void task_module_ptr_init(const struct task *task, void *mod_p_space);
static int task_module_ptr_inherit(const struct task *task,
    const struct task *parent);

TASK_RESOURCE_DECLARE(static, task_resource_module_ptr, const struct mod *,
    .init = task_module_ptr_init, .inherit = task_module_ptr_inherit, );

static void task_module_ptr_init(const struct task *task, void *mod_p_space) {
	const struct mod **mod_p = mod_p_space;

	*mod_p = NULL;
}

static int task_module_ptr_inherit(const struct task *task,
    const struct task *parent) {
	const struct mod **task_mod_p = task_resource(task, &task_resource_module_ptr);
	const struct mod **parent_mod_p = task_resource(parent,
	    &task_resource_module_ptr);

	*task_mod_p = *parent_mod_p;
	return 0;
}

void task_self_module_ptr_set(const struct mod *mod) {
	const struct mod **mod_p = task_self_resource(&task_resource_module_ptr);

	*mod_p = mod;
}

const struct mod *task_module_ptr_get(struct task *task) {
	const struct mod **mod_p = task_resource(task, &task_resource_module_ptr);

	return *mod_p;
}
