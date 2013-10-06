/**
 * @file
 * @brief
 *
 * @date 22.10.2012
 * @author Alexander Kalmuk
 * @author Eldar Abusalimov
 */

#include <kernel/task/rt_signal.h>

#include "common.h"

#include <math.h>
#include <signal.h>

#include <kernel/task/task_table.h>
#include <kernel/irq_lock.h>
#include <mem/objalloc.h>
#include <util/array.h>
#include <util/dlist.h>

struct rtsig_data {
	union sigval sigval;
	struct dlist_head sig_link;
};

OBJALLOC_DEF(rtsig_data_pool, struct rtsig_data,
		TASK_RTSIG_CNT * TASK_RTSIG_CNT_PER_TYPE);

void task_rtsignal_send(struct task *task, int sig, union sigval value) {
	struct task_signal_table *sig_table = task->signal_table;
	struct rtsig_data *data;
	int rtsig = sig - TASK_SIGRTMIN;

	data = objalloc(&rtsig_data_pool);
	if (!data)
		/* XXX then what? -- Eldar */
		assert(0, "FIXME");

	dlist_head_init(&data->sig_link);
	data->sigval = value;

	irq_lock();
	dlist_add_prev(&data->sig_link, &sig_table->rtsig_data[rtsig]);
	irq_unlock();
}

void task_rtsignal_handle(void) {
	struct task_signal_table *sig_table = task_self()->signal_table;
	struct rtsig_data *data, *nxt;

	for (int rtsig = 0; rtsig < TASK_RTSIG_CNT; ++rtsig) {
		task_rtsignal_hnd_t hnd;
		struct dlist_head *sig_list;

		hnd = sig_table->rt_hnd[rtsig];
		if (!hnd)
			continue;

		sig_list = &sig_table->rtsig_data[rtsig];
		dlist_foreach_entry(data, nxt, sig_list, sig_link) {
			union sigval sigval = data->sigval;

			irq_lock();
			dlist_del(&data->sig_link);
			irq_unlock();

			objfree(&rtsig_data_pool, data);

			hnd(rtsig + TASK_SIGRTMIN, sigval);
		}

		assert(dlist_empty(sig_list));
	}
}

SIGNAL_HANDLE_ENTRY(task_rtsignal_handle);
