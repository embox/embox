/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 * 			- split signals into standard signals and real-time signals
 * @date    22.10.2012
 */

#include <math.h>
#include <kernel/task/task_table.h>
#include <kernel/task/signal.h>
#include <kernel/task/std_signal.h>
#include <kernel/thread/sched.h>
#include "common.h"
#include <util/array.h>

ARRAY_SPREAD_DEF(global_sig_hnd_t, __signal_handlers_array);

extern void context_enter_frame(struct context *ctx, void *pc);
extern void context_push_stack(struct context *ctx, void *arg, size_t n);

int task_some_thd_run(struct task *task) {
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

void SIG_DFL(int sig) {
	task_terminate(sig);
}

static void task_signal_table_init(struct task *task, void *_signal_table) {
	int sig;

	struct task_signal_table *sig_table =
		(struct task_signal_table *) _signal_table;

	for (sig = 0; sig < TASK_SIGNAL_MAX_N; sig++) {
		task_signal_table_set(sig_table, sig, task_terminate);
	}

	for (sig = 0; sig < TASK_RTSIG_CNT; sig++) {
		sig_table->rt_hnd[sig] = NULL;
		dlist_init(&sig_table->rtsig_data[sig]);
	}

	task->signal_table = sig_table;
}

static const struct task_resource_desc signal_resource = {
	.init = task_signal_table_init,
	.resource_size = sizeof(struct task_signal_table),
};

void task_signal_hnd(void) {
	global_sig_hnd_t hnd;

	sched_unlock();
	{
		array_foreach(hnd, __signal_handlers_array, ARRAY_SPREAD_SIZE(__signal_handlers_array)) {
			if (hnd) {
				hnd();
			}
		}
	}
	sched_lock();
}

static int notify_hnd(struct thread *prev, struct thread *next) {
	context_enter_frame(&next->context, task_signal_hnd);
	return 0;
}

TASK_RESOURCE_DESC(&signal_resource);

TASK_RESOURCE_NOTIFY(notify_hnd);
