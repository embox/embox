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
	char *argv[ARGS_QUANTITY + 1]; /* null terminated */
	char path[ARG_LENGTH]; /* FIXME PATH_MAX */
};

TASK_RESOURCE_DEF(task_argv_desc, struct task_argv);

static int task_argv_exec(const struct task *task, const char *path, char *const argv[]) {
	int i;
	struct task_argv *task_argv;

	assert(path);

	task_argv = task_resource_argv(task);
	task_argv->argc = argv_to_argc(argv);
	assert(task_argv->argc <= ARGS_QUANTITY);

	for (i = 0; i < task_argv->argc; i++) {
		strncpy(task_argv->argv_buff[i], argv[i], sizeof(*task_argv->argv_buff));
		task_argv->argv_buff[i][sizeof(*task_argv->argv_buff) - 1] = '\0';
		task_argv->argv[i] = task_argv->argv_buff[i];
	}
	task_argv->argv[i] = NULL;

/* FIXME #pragma GCC diagnostic ignored "-Wstringop-truncation" */
#if defined(__GNUC__) && (__GNUC__ > 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif

	strncpy(task_argv->path, path, sizeof(task_argv->path));
	task_argv->path[sizeof(task_argv->path) - 1] = '\0';

#if defined(__GNUC__) && (__GNUC__ > 7)
#pragma GCC diagnostic pop
#endif

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

void task_resource_argv_insert(const struct task *task, const char *arg, int index) {
	struct task_argv *task_argv;

	task_argv = task_resource_argv(task);
	assert(task_argv->argc < ARGS_QUANTITY);

	strncpy(task_argv->argv_buff[task_argv->argc], arg, sizeof(*task_argv->argv_buff));
	task_argv->argv_buff[task_argv->argc][sizeof(*task_argv->argv_buff) - 1] = '\0';

	memmove(task_argv->argv + index + 1, task_argv->argv + index,
			(task_argv->argc - index + 1) * sizeof *task_argv->argv);

	task_argv->argv[index] = task_argv->argv_buff[task_argv->argc++];
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
