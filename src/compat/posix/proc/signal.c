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

int sigaction(int sig, const struct sigaction *restrict act,
		struct sigaction *restrict oact) {
	struct signal_table *sig_tab = task_self()->signal_table;

	if (!check_range(sig, SIGSTD_MIN, SIGSTD_MAX))
		return SET_ERRNO(EINVAL);

	if (oact) {
		sighandler_t ofunc = sig_tab->sigstd_handlers[sig - SIGSTD_MIN];

		if (ofunc == sighandler_default) {
			ofunc = SIG_DFL;
		} else if (ofunc == sighandler_ignore) {
			ofunc = SIG_IGN;
		}

		oact->sa_handler = ofunc;
	}

	if (act) {
		sighandler_t func = act->sa_handler;

		if (func == SIG_DFL) {
			func = sighandler_default;
		} else if (func == SIG_IGN || func == SIG_ERR) {
			func = sighandler_ignore;
		}

		sig_tab->sigstd_handlers[sig - SIGSTD_MIN] = func;
	}

	return 0;
}

sighandler_t signal(int sig, sighandler_t func) {
	struct sigaction act  = { 0 };
	struct sigaction oact = { 0 };
	int err;

	act.sa_handler = func;

	err = sigaction(sig, &act, &oact);
	if (err) {
		SET_ERRNO(err);
		return SIG_ERR;
	}

	return oact.sa_handler;
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
