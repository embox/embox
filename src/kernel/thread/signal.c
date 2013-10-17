/**
 * @file
 * @brief Current thread signal handling.
 *
 * @date 14.10.2013
 * @author Eldar Abusalimov
 */

#include <kernel/task.h>
#include <kernel/thread.h>
#include <kernel/thread/signal.h>


void thread_signal_handle(void) {
	struct thread *thread = thread_self();
	struct task   *task   = thread->task;

	struct signal_data *sig_data = &thread->signal_data;
	struct sigaction *sig_table = task->sig_table;

	sigrt_handle(&sig_data->sigrt_data, sig_table);
	sigstd_handle(&sig_data->sigstd_data, sig_table);

}
