/**
 * @file
 * @brief
 *
 * @date 08.10.2012
 * @author Anton Kozlov
 * @author Eldar Abusalimov
 */

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <pthread.h>

#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/signal.h>

#include <util/math.h>

static void sighandler_default(int sig) {
	task_exit(NULL);
}

static void sighandler_ignore(int sig) {
	/* do nothing */
}

sighandler_t signal(int sig, sighandler_t func) {
	struct signal_table *sig_tab = task_self()->signal_table;
	sighandler_t old_func;

	if (!check_range(sig, SIGSTD_MIN, SIGSTD_MAX)) {
		SET_ERRNO(EINVAL);
		return NULL;
	}

	old_func = sig_tab->sigstd_handlers[sig];

	if (func == SIG_DFL) {
		func = sighandler_default;
	} else if (func == SIG_IGN || func == SIG_ERR) {
		func = sighandler_ignore;
	}

	sig_tab->sigstd_handlers[sig] = func;

	return old_func;
}

int sigqueue(int tid, int sig, const union sigval value) {
	struct task *task;
	struct signal_data *sig_data;
	int err;

	task = task_table_get(tid);
	if (!task)
		return SET_ERRNO(ESRCH);

	sig_data = &task->main_thread->signal_data;
	err = sigrt_raise(&sig_data->sigrt_data, sig, value);
	if (err)
		return SET_ERRNO(err);

	sched_post_switch();  /* This shouldn't be here, I think  -- Eldar */

	return 0;
}


int pthread_kill(pthread_t thread, int sig) {
	struct signal_data *sig_data;
	int err;

	assert(thread);

	sig_data = &thread->signal_data;
	err = sigstd_raise(&sig_data->sigstd_data, sig);
	if (err)
		return SET_ERRNO(err);

	sched_post_switch();

	return 0;
}

int kill(int tid, int sig) {
	struct task *task;

	task = task_table_get(tid);
	if (!task)
		return SET_ERRNO(ESRCH);

	return pthread_kill(task->main_thread, sig);
}
