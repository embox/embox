/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    22.10.2012
 */

#include <math.h>
#include <kernel/task/task_table.h>
#include <kernel/task/rt_signal.h>
#include <kernel/thread/sched.h>
#include "common.h"
#include <mem/objalloc.h>
#include <kernel/irq_lock.h>
#include <util/array.h>
#include <util/dlist.h>
#include <signal.h>

struct rtsig_data {
	union sigval data;
	struct dlist_head sig_link;
};

OBJALLOC_DEF(rtsig_data_pool, struct rtsig_data, TASK_RTSIG_CNT * TASK_RTSIG_CNT_PER_TYPE);

void task_rtsignal_send(struct task *task, int sig, const union sigval value) {
	struct rtsig_data *data = objalloc(&rtsig_data_pool);

	dlist_head_init(&data->sig_link);
	data->data = value;

	irq_lock();
	dlist_add_prev(&data->sig_link, &task->signal_table->rtsig_data[sig - TASK_SIGRTMIN]);
	irq_unlock();
}

void task_rtsignal_handle(void) {
	struct task_signal_table *sig_table = task_self()->signal_table;
	struct rtsig_data *cur, *nxt;
	int sig;
	task_rtsignal_hnd_t hnd;

	for (sig = TASK_SIGRTMIN; sig <= TASK_SIGRTMAX; sig++) {
		if (task_rtsignal_table_get(task_self()->signal_table, sig)) {
			dlist_foreach_entry(cur, nxt, &sig_table->rtsig_data[sig - TASK_SIGRTMIN], sig_link) {
				hnd = task_rtsignal_table_get(task_self()->signal_table, sig);
				hnd(sig, cur->data);
				dlist_del(&cur->sig_link);
				objfree(&rtsig_data_pool, cur);
			}

			assert(dlist_empty(&sig_table->rtsig_data[sig - TASK_SIGRTMIN]));
		}
	}
}

SIGNAL_HANDLE_ENTRY(task_rtsignal_handle);
