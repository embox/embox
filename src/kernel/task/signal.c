/**
 * @file
 * @brief
 *
 * @date    22.10.2012
 * @author  Anton Kozlov
 * @author  Alexander Kalmuk
 * 			- split signals into standard signals and real-time signals
 * @author  Eldar Abusalimov
 */

#include "common.h"

#include <signal.h>


static void task_sig_handler_terminate(int sig) {
	task_exit(NULL);
}

static void task_sig_table_init(struct task *task, void *resource) {
	struct sigaction *sig_table = resource;

	for (int sig = 0; sig < _SIG_TOTAL; ++sig) {
		sig_table[sig].sa_handler = task_sig_handler_terminate;
	}

	task->sig_table = sig_table;
}

static const struct task_resource_desc signal_resource = {
	.init = task_sig_table_init,
	.resource_size = sizeof(struct sigaction) * _SIG_TOTAL,
};

TASK_RESOURCE_DESC(&signal_resource);

