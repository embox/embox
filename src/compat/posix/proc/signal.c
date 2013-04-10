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
#include <kernel/task/rt_signal.h>
#include <kernel/task/std_signal.h>
#include <kernel/task/task_table.h>

static void sighnd_default(int sig) {
	task_exit(NULL);
}

static void sighnd_ignore(int sig) {
	/* do nothing */
}

void (*signal(int sig, void (*func)(int)))(int) {
	void (*old_func)(int) = task_self_signal_get(sig);

	if (func == SIG_DFL) {
		func = sighnd_default;
	} else if (func == SIG_IGN || func == SIG_ERR) {
		func = sighnd_ignore;
	}

	task_self_signal_set(sig, func);

	return old_func;
}

int sigqueue(int tid, int sig, const union sigval value) {
	struct task *task = task_table_get(tid);

	if (task == NULL) {
		SET_ERRNO(ESRCH);
		return -1;
	}

	task_rtsignal_send(task, sig, value);

	return 0;
}

int kill (int tid, int sig) {
	struct task *task = task_table_get(tid);

	if (task == NULL) {
		SET_ERRNO(ESRCH);
		return -1;
	}

	task_stdsig_send(task, sig);

	return 0;
}
