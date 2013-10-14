/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    08.10.2012
 */

#include <errno.h>
#include <signal.h>
#include <stddef.h>

#include <kernel/task.h>
#include <kernel/thread/signal.h>
// #include <kernel/task/rt_signal.h>
// #include <kernel/task/std_signal.h>
// #include <kernel/task/task_table.h>

#if 0
static void sighnd_default(int sig) {
	task_exit(NULL);
}

static void sighnd_ignore(int sig) {
	/* do nothing */
}

void (*signal(int sig, void (*func)(int)))(int) {
	struct signal_table *sig_tab = task_self()->signal_table;
	void (*old_func)(int) = sig_tab->handlers[sig];

	if (func == SIG_DFL) {
		func = sighnd_default;
	} else if (func == SIG_IGN || func == SIG_ERR) {
		func = sighnd_ignore;
	}

	sig_tab->hnd[sig] = func;

	return old_func;
}

int sigqueue(int tid, int sig, const union sigval value) {
	struct task *task = task_table_get(tid);

	if (task == NULL) {
		SET_ERRNO(ESRCH);
		return -1;
	}

	// sigrt_raise(task, sig, value);

	return 0;
}

int kill (int tid, int sig) {
	struct task *task = task_table_get(tid);

	if (task == NULL) {
		SET_ERRNO(ESRCH);
		return -1;
	}

	// task_stdsig_send(task, sig);

	return 0;
}
#endif
