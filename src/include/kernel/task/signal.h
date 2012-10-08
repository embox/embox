/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include <types.h>
#include <kernel/task.h>

#ifndef SRC_INCLUDE_KERNEL_TASK_SIGNAL_H_
#define SRC_INCLUDE_KERNEL_TASK_SIGNAL_H_

typedef void (*task_signal_hnd_t)(int sig);

#define TASK_SIGNAL_MAX_N 16

struct task_signal_table {
	uint32_t sig_mask;
	char last_sig;
	task_signal_hnd_t hnd[TASK_SIGNAL_MAX_N];
};

static inline void task_signal_table_set(struct task_signal_table *table, int sig, task_signal_hnd_t hnd) {
	table->hnd[sig] = hnd;
}

static inline task_signal_hnd_t task_signal_table_get(struct task_signal_table *table, int sig) {
	return table->hnd[sig];
}

static inline task_signal_hnd_t task_self_signal_get(int sig) {
	struct task_signal_table *sig_table = task_self()->signal_table;

	return task_signal_table_get(sig_table, sig);
}

static inline void task_self_signal_set(int sig, task_signal_hnd_t hnd) {
	struct task_signal_table *sig_table = task_self()->signal_table;

	task_signal_table_set(sig_table, sig, hnd);
}

extern void task_signal_send(struct task *task, int sig);

#endif /* SRC_INCLUDE_KERNEL_TASK_SIGNAL_H_ */
