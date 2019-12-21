/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.06.2014
 */

#include <kernel/task/resource.h>

#include <sys/stat.h>

#define DEFAULT_UMASK OPTION_GET(NUMBER,default_umask)

static void task_umask_init(const struct task *task, void *space);
static int task_umask_inherit(const struct task *task, const struct task *parent);

TASK_RESOURCE_DECLARE(static,
		task_resource_umask_desc,
		mode_t,
	.init = task_umask_init,
	.inherit = task_umask_inherit,
);

static void task_umask_init(const struct task *task, void *space) {
	mode_t *umask_p = space;

	*umask_p = DEFAULT_UMASK;
}

static int task_umask_inherit(const struct task *task, const struct task *parent) {
	mode_t *task_umask_p = task_resource(task, &task_resource_umask_desc);
	mode_t *parent_umask_p = task_resource(parent, &task_resource_umask_desc);

	*task_umask_p = *parent_umask_p;

	return 0;
}

mode_t *task_resource_umask(const struct task *task) {
	return task_resource(task, &task_resource_umask_desc);
}
