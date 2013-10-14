/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 * 			- split signals into standard signals and real-time signals
 * @author  Eldar Abusalimov
 * @date    22.10.2012
 */

#include <math.h>

#include "common.h"

#include <kernel/thread/signal.h>


static void task_terminate_sigstd_handler(int sig) {
	task_exit(NULL);
}

static void task_terminate_sigrt_handler(int sig, union sigval sigval) {
	task_exit(NULL);
}

static void task_signal_table_init(struct task *task, void *resource) {
	struct signal_table *sig_tab = resource;

	for (int sig = 0; sig < SIGSTD_CNT; ++sig) {
		sig_tab->sigstd_handlers[sig] = task_terminate_sigstd_handler;
	}

	for (int sig = 0; sig < SIGRT_CNT; ++sig) {
		sig_tab->sigrt_handlers[sig] = task_terminate_sigrt_handler;
	}

	task->signal_table = sig_tab;
}

static const struct task_resource_desc signal_resource = {
	.init = task_signal_table_init,
	.resource_size = sizeof(struct signal_table),
};

TASK_RESOURCE_DESC(&signal_resource);

