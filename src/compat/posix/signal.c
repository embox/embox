/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    08.10.2012
 */

#include <errno.h>
#include <signal.h>
#include <kernel/task/signal.h>
#include <kernel/task/task_table.h>

void (*signal(int sig, void (*func)(int)))(int) {
	void (*old_func)(int) = task_self_signal_get(sig);

	task_self_signal_set(sig, func);

	return old_func;
}

int kill (int tid, int sig) {
	struct task *task = task_table_get(tid);

	if (task == NULL) {
		SET_ERRNO(ESRCH);
		return -1;
	}

	task_signal_send(task, sig);

	return 0;
}
