/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include <math.h>
#include <kernel/task/task_table.h>
#include <kernel/task/signal.h>
#include <kernel/thread/sched.h>
#include "common.h"

extern void context_enter_frame(struct context *ctx, void (*pc)(void));

static int task_some_thd_run(struct task *task);

void task_signal_send(struct task *task, int sig) {

	task->signal_table->sig_mask |= 1 << sig;

	task_some_thd_run(task);
}

static int task_some_thd_run(struct task *task) {
	struct thread *th;
	int res = -1;

	list_for_each_entry(th, &task->threads, task_link) {
		if ((res = sched_tryrun(th)) != -1) {
			break;
		}
	}

	return res;

}

static void task_terminate(int sig) {
	task_exit(NULL);
}

static void task_global_sig_handler(void) {
	struct task_signal_table *sig_table = task_self()->signal_table;
	int sig_occured = sig_table->last_sig;

	sched_unlock();

	task_signal_table_get(sig_table, sig_occured)(sig_occured);

	sched_lock();
}

static void task_signal_table_init(struct task *task, void *_signal_table) {
	int sig;

	struct task_signal_table *sig_table = (struct task_signal_table *) _signal_table;

	for (sig = 0; sig < TASK_SIGNAL_MAX_N; sig++) {
		task_signal_table_set(sig_table, sig, task_terminate);
	}

	task->signal_table = sig_table;
}

static const struct task_resource_desc signal_resource = {
	.init = task_signal_table_init,
	.resource_size = sizeof(struct task_signal_table),
};

static int notify_hnd(struct thread *prev, struct thread *next) {
	struct task_signal_table *sig_table= next->task->signal_table;
	int sig = (sig_table->sig_mask ? blog2(sig_table->sig_mask) : 0);

	if (sig) {
		sig_table->sig_mask &= ~(1 << sig);
		sig_table->last_sig = sig;
		context_enter_frame(&next->context, task_global_sig_handler);
	}


	return 0;
}

TASK_RESOURCE_DESC(&signal_resource);

TASK_RESOURCE_NOTIFY(notify_hnd);
