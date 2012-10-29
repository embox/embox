/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include <math.h>
#include <kernel/task/task_table.h>
#include <kernel/task/std_signal.h>
#include "common.h"
#include <util/array.h>

extern int task_some_thd_run(struct task *task);

void task_stdsig_send(struct task *task, int sig) {

	task->signal_table->sig_mask |= 1 << sig;

	task_some_thd_run(task);
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
