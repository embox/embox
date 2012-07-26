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

void kill(int tid, int sig) {
	struct task *task = task_table_get(tid);
	struct task_signal_table *sig_table = task->signal_table;
	struct thread *hnd_thread = member_cast_out(task->threads.next, struct thread, task_link);

	sig_table->sig_mask |= 1 << sig;

	/*sched_setrun(hnd_thread);*/
}

void signal(int sig, void (*hnd)(int)) {
	struct task *task = task_self();
	struct task_signal_table *sig_table = task->signal_table;

	task_signal_table_set(sig_table, sig, hnd);
}

static void task_global_sig_handler(void) {
	struct task_signal_table *sig_table = task_self()->signal_table;

	task_signal_table_get(sig_table, sig_table->last_sig)(sig_table->last_sig);
}

static void task_signal_table_init(struct task *task, void *_signal_table) {
	struct task_signal_table *sig_table = (struct task_signal_table *) _signal_table;

	task->signal_table = sig_table;
}

static void task_signal_table_inherit(struct task *task, struct task *parent_task) {

}

static void task_signal_table_deinit(struct task *task) {

}

static const struct task_resource_desc signal_resource = {
	.init = task_signal_table_init,
	.inherit = task_signal_table_inherit,
	.deinit = task_signal_table_deinit,
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
