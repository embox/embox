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
#include <unistd.h>

extern int task_create_with_io(struct task **new, struct task *parent, FILE *stdin, FILE *stdout, FILE *stderr);
extern int thread_create_task(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg, struct task *tsk);

EMBOX_EXAMPLE(task_io_example_run);

static void *thread_handler(void *data) {
	char ch = 'a';
	int count = 10;
	while (count--) {
		write(0, &ch, 1);
	}
	return thread_self();
}

static int task_io_example_run(int argc, char **argv) {
	struct task *new_task;
	struct thread *thd;
	void *ret;
	FILE *file = fopen("/dev/diag", "rw");

	printf("file opened\n");
	task_create_with_io(&new_task, NULL, file, file, file);

	thread_create_task(&thd, 0, thread_handler, NULL, new_task);

	thread_join(thd, &ret);
	return 0;
}
