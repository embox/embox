/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    18.02.2013
 */

#include <config/embox/kernel/task/resource/security.h>
#include <framework/mod/options.h>
#include <string.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/security.h>

#define MODOPS_SECURITY_SIZE OPTION_GET(NUMBER, security_size)

TASK_RESOURCE_DEF(task_security_desc, char [MODOPS_SECURITY_SIZE]);

static void task_security_init(const struct task *task,
		void *security_space) {
	void *security;

	assert(security_space == task_resource_security(task));

	security = security_space;
	assert(security != NULL);

	memset(security, 0, MODOPS_SECURITY_SIZE);
}

static int task_security_inherit(const struct task *task,
		const struct task *parent) {
	struct task_env *task_security, *parent_security;

	task_security = task_resource_security(task);
	assert(task_security != NULL);

	parent_security = task_resource_security(parent);
	assert(parent_security != NULL);

	memcpy(task_security, parent_security, MODOPS_SECURITY_SIZE);

	return 0;
}

static size_t task_security_offset;

static const struct task_resource_desc task_security_desc = {
	.init = task_security_init,
	.inherit = task_security_inherit,
	.resource_size = MODOPS_SECURITY_SIZE,
	.resource_offset = &task_security_offset
};

void * task_resource_security(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_security_offset;
}
