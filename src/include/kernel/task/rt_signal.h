/**
 * @file
 * @brief Functions to operate with real-time signals
 *
 * @author  Alexander Kalmuk
 * @date    22.10.2012
 */

#ifndef SRC_INCLUDE_KERNEL_TASK_RT_SIGNAL_H_
#define SRC_INCLUDE_KERNEL_TASK_RT_SIGNAL_H_

#include "signal.h"

/* Real-time signals support */

extern void task_rtsignal_send(struct task *task, int sig, const union sigval value);
extern void task_rtsignal_handle(void);

static inline void task_rtsignal_table_set(struct task_signal_table *table, int sig, task_rtsignal_hnd_t hnd) {
	table->rt_hnd[sig - TASK_SIGRTMIN] = hnd;
}

static inline task_rtsignal_hnd_t task_rtsignal_table_get(struct task_signal_table *table, int sig) {
	return table->rt_hnd[sig - TASK_SIGRTMIN];
}

static inline task_rtsignal_hnd_t task_rtself_signal_get(int sig) {
	struct task_signal_table *sig_table = task_self()->signal_table;

	return task_rtsignal_table_get(sig_table, sig);
}

static inline void task_self_rtsignal_set(int sig, task_rtsignal_hnd_t hnd) {
	struct task_signal_table *sig_table = task_self()->signal_table;

	task_rtsignal_table_set(sig_table, sig, hnd);
}

#endif /* SRC_INCLUDE_KERNEL_TASK_RT_SIGNAL_H_ */
