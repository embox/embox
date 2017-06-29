/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    28.05.2014
 */

#include <kernel/task.h>
#include <unistd.h>
#include <sys/wait.h>

struct exec_data {
	int argc;
	char **argv;
};

static void *exec_helper_trampoline(void *arg) {
	struct exec_data *data = arg;

	execv(data->argv[0], data->argv);

	_exit(1);
}

int main(int argc, char *argv[]) {
	int res;
	struct exec_data edata;

       	edata.argc = argc - 1;
	edata.argv = argv + 1;

	new_task("", exec_helper_trampoline, &edata);

	wait(&res);
	return res;
}
