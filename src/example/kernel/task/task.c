/**
 * @file
 * @brief open file, associate it with stdio of new task
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>
#include <kernel/task.h>
#include <kernel/thread/api.h>
#include <posix/unistd.h>

EMBOX_EXAMPLE(task_io_example_run);

static void *thread_handler(void *data) {
	FILE *file = fopen("/dev/uart", "rw");
	reopen(0, file);
	reopen(1, file);
	reopen(2, file);

	char ch = 'a';
	int count = 10;

	while (count--) {
		write(0, &ch, 1);
	}
	return thread_self();
}

static int task_io_example_run(int argc, char **argv) {
	//struct task *new_task;
	struct thread *thd;
	void *ret;

	printf("thread created\n");

	thread_create(&thd, THREAD_FLAG_IN_NEW_TASK, thread_handler, NULL);

	thread_join(thd, &ret);
	return 0;
}
