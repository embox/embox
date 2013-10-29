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


void thread_signal_handle(void) {
	struct thread *thread = thread_self();
	struct task   *task   = thread->task;

	struct sigstate *sigstate = &thread->sigstate;
	struct sigaction *sig_table = task->sig_table;

	sighandler_t handler;
	siginfo_t info;
	int sig;

	while ((sig = sigstate_receive(sigstate, &info))) {

		// TODO locks?
		handler = sig_table[sig].sa_handler;
		assert(handler && "there must be at least a fallback handler");

		// TODO passing info
		handler(sig);
	}
}
