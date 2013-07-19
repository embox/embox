/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include <math.h>
#include <util/array.h>

#include <kernel/thread.h>
#include <kernel/sched.h>

#include <kernel/task/task_table.h>
#include <kernel/task/std_signal.h>
#include "common.h"


void task_stdsig_send(struct task *task, int sig) {
	struct thread *th, *tmp;
	int res = -1;

	task->signal_table->sig_mask |= 1 << sig;

	if(-1 != (res = sched_tryrun(task->main_thread))) {
		return;
	}

	dlist_foreach_entry(th, tmp, &task->main_thread->thread_task_link, thread_task_link) {
		if (-1 != (res = sched_tryrun(th))) {
			return;
		}
	}
}

void task_stdsig_handle(void) {
	struct task_signal_table *sig_table= task_self()->signal_table;
	int sig = (sig_table->sig_mask ? blog2(sig_table->sig_mask) : 0);

	/*TODO handle all standard signals */
	if (sig) {
		sig_table->sig_mask &= ~(1 << sig);
		sig_table->last_sig = sig;
		task_signal_table_get(task_self()->signal_table, sig)(sig);
	}
}

SIGNAL_HANDLE_ENTRY(task_stdsig_handle);
