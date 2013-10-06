/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */


#include <kernel/thread.h>
#include <kernel/sched.h>

#include <kernel/task/task_table.h>
#include <kernel/task/std_signal.h>
#include "common.h"

#include <util/bit.h>


void task_stdsig_send(struct task *task, int sig) {
	struct thread *th, *tmp;
	int res = -1;

	task->signal_table->sig_mask |= 1 << sig;

	res = sched_signal(task->main_thread, sig);
	if (res != -1)
		return;

	dlist_foreach_entry(th, tmp, &task->main_thread->thread_link, thread_link) {
		res = sched_signal(th, sig);
		if (res != -1)
			return;
	}
}

void task_stdsig_handle(void) {
	struct task_signal_table *sig_table = task_self()->signal_table;
	task_signal_hnd_t hnd;
	int sig;

	if (!sig_table->sig_mask)
		return;

	sig = bit_ctz(sig_table->sig_mask);
	hnd = sig_table->hnd[sig];

	/*TODO handle all standard signals */
	if (sig) {
		sig_table->sig_mask &= ~(1 << sig);
		hnd(sig);
	}
}

SIGNAL_HANDLE_ENTRY(task_stdsig_handle);
