/**
 * @file
 * @brief Current thread signal handling.
 *
 * @date 14.10.2013
 * @author Eldar Abusalimov
 */

#include <signal.h>

#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/thread/signal.h>
#include <kernel/task/resource/sig_table.h>

void thread_signal_handle(void) {
	struct thread *thread;
	struct task *task;
	struct sigstate *sigstate;
	struct sigaction *sig_table;
	siginfo_t info;
	int sig;

	thread = thread_self();
	assert(thread != NULL);

	task = thread->task;
	assert(task != NULL);

	sigstate = &thread->sigstate;

	sig_table = task_resource_sig_table(task);
	assert(sig_table != NULL);

	while ((sig = sigstate_receive(sigstate, &info))) {
		struct sigaction *act = sig_table + sig;

		// TODO locks?
		if (act->sa_flags & SA_SIGINFO) {
			assert(act->sa_sigaction && "expected at least a fallback handler");
 			act->sa_sigaction(sig, &info, NULL);

		} else {
			assert(act->sa_handler && "expected at least a fallback handler");
 			act->sa_handler(sig);
		}

	}
}
