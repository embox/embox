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

static int task_argv_exec(const struct task *task, const char *path, char *const argv[]) {
	int i;
	struct task_argv *task_argv;

	assert(path);

	task_argv = task_resource_argv(task);
	task_argv->argc = argv_to_argc(argv);

	for (i = 0; i < task_argv->argc; i++) {
		strncpy(task_argv->argv_buff[i], argv[i], sizeof(*task_argv->argv_buff));
		task_argv->argv_buff[i][sizeof(*task_argv->argv_buff) - 1] = '\0';
		task_argv->argv[i] = task_argv->argv_buff[i];
	}

	strncpy(task_argv->path, path, sizeof(task_argv->path));
	task_argv->path[sizeof(task_argv->path) - 1] = '\0';

	return 0;
}

static size_t task_argv_offset;

static const struct task_resource_desc task_argv_desc = {
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

int argv_to_argc(char *const argv[]) {
	int argc;

	if (argv == NULL)
		return 0;

	for (argc = 0; argv[argc]; argc ++) {
	}

	return argc;
}
