/**
 * @file
 *
 * @date Jun 11, 2014
 * @author: Anton Bondarev
 */


#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_argv.h>

#include <framework/mod/options.h>

#define ARGS_QUANTITY OPTION_GET(NUMBER,args_quantity)
#define ARG_LENGTH    OPTION_GET(NUMBER, arg_length)

struct task_argv {
	int argc;
	char argv_buff[ARGS_QUANTITY][ARG_LENGTH];
	char *argv[ARGS_QUANTITY];
	char path[ARG_LENGTH];
};

TASK_RESOURCE_DEF(task_argv_desc, struct task_argv);


extern struct task_argv *task_resource_argv(const struct task *task);

static void task_argv_init(const struct task *task, void *buff) {
}

static int task_argv_inherit(const struct task *task,
		const struct task *parent) {
	return 0;
}

static int task_argv_exec(const struct task *task, void *buff) {
	int argc;
	struct task_param *task_param;
	struct task_argv *task_argv;

	assert(buff);
	task_param = buff;

	task_argv = task_resource_argv(task);

	for (argc = task_param->argc; argc != 0; argc --) {
		strncpy(task_argv->argv_buff[argc - 1], task_param->argv[argc - 1], sizeof(*task_argv->argv_buff));
		task_argv->argv[argc - 1] = task_argv->argv_buff[argc - 1];
	}
	task_argv->argc = task_param->argc;

	strncpy(task_argv->path, task_param->path, sizeof(task_argv->path));


	return 0;

}

static size_t task_argv_offset;

static const struct task_resource_desc task_argv_desc = {
	.init = task_argv_init,
	.inherit = task_argv_inherit,
	.resource_size = sizeof(struct task_argv),
	.resource_offset = &task_argv_offset,
	.exec = task_argv_exec
};

struct task_argv *task_resource_argv(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_argv_offset;
}

int task_resource_argv_argc(const struct task *task) {
	return task_resource_argv(task)->argc;
}

char **task_resource_argv_argv(const struct task *task) {
	return task_resource_argv(task)->argv;
}

char *task_resource_argv_path(const struct task *task) {
	return task_resource_argv(task)->path;
}
