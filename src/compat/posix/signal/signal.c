/**
 * @file
 * @brief
 *
 * @date 08.10.2012
 * @author Anton Kozlov
 * @author Eldar Abusalimov
 */

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stddef.h>
#include <pthread.h>

#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/signal.h>
#include <kernel/task/resource/sig_table.h>

#include <util/math.h>

static void sighandler_default(int sig) {
	task_exit(NULL);
}

static void sighandler_ignore(int sig) {
	/* do nothing */
}

int sigaction(int sig, const struct sigaction *restrict act,
		struct sigaction *restrict oact) {
	struct sigaction *sig_table = task_self_resource_sig_table();

	if (!check_range(sig, 0, _SIG_TOTAL) || IS_UNMODIFIABLE_SIGNAL(sig))
		return SET_ERRNO(EINVAL);

	if (oact) {
		sighandler_t ofunc = sig_table[sig].sa_handler;
		memcpy(oact, &sig_table[sig], sizeof(struct sigaction));

		if (ofunc == sighandler_default) {
			ofunc = SIG_DFL;
		} else if (ofunc == sighandler_ignore) {
			ofunc = SIG_IGN;
		}

		oact->sa_handler = ofunc;
	}

	if (act) {
		sighandler_t func = act->sa_handler;
		memcpy(&sig_table[sig], act, sizeof(struct sigaction));

		if (func == SIG_DFL) {
			func = sighandler_default;
		} else if (func == SIG_IGN || func == SIG_ERR) {
			func = sighandler_ignore;
		}

		sig_table[sig].sa_handler = func;
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
		return SIG_ERR;
	}

	return oact.sa_handler;
}

int sigqueue(int tid, int sig, const union sigval value) {
	struct task *task;
	struct sigstate *sigstate;
	siginfo_t info;
	int err;

	task = task_table_get(tid);
	if (!task)
		return SET_ERRNO(ESRCH);

	sigstate = &task_get_main(task)->sigstate;

	// TODO prepare it
	info.si_value = value;

	err = sigstate_send(sigstate, sig, &info);
	if (err)
		return SET_ERRNO(err);

	sched_signal(&task_get_main(task)->schedee);

	return 0;
}


int pthread_kill(pthread_t thread, int sig) {
	struct sigstate *sigstate;
	int err;

	assert(thread);

	sigstate = &thread->sigstate;
	err = sigstate_send(sigstate, sig, NULL);
	if (err)
		return SET_ERRNO(err);

	sched_signal(&thread->schedee);

	return 0;
}

int kill(int tid, int sig) {
	struct task *task;

	if (sig == 0) {
		/* If sig is 0 (the null signal), error checking is performed
		 but no signal is actually sent. The null signal can be used to
		  check the validity of pid. */
		return 0;
	}

	if (tid <= 0) {
		/* process group is the target, NYI */
		return -ENOSYS;
	}

	task = task_table_get(tid);
	if (!task) {
		return SET_ERRNO(ESRCH);
	}
	
	return pthread_kill(task_get_main(task), sig);
}

int raise(int signo) {
	return pthread_kill(thread_self(), signo);
}
