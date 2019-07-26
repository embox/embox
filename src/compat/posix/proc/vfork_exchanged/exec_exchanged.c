/**
 * @file
 *
 * @date May 21, 2014
 * @author: Anton Bondarev
 */
#include <stddef.h>

#include <cmd/shell.h>
#include <kernel/task.h>
#include <hal/vfork.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/task_argv.h>

extern int exec_call();

void *task_exec_callback(void *arg) {
	int res;

	res = exec_call();

	return (void*) (uintptr_t) res;
}
