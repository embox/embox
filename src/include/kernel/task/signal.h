/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.06.2012
 */

#include <types.h>

typedef void (*task_signal_hnd)(int sig);

#define TASK_SIGNAL_MAX_N 16

struct task_signal_table {
	uint32_t sig_mask;
	char last_sig;
	task_signal_hnd hnd[TASK_SIGNAL_MAX_N];
};

static inline void task_signal_table_set(struct task_signal_table *table, int sig, task_signal_hnd hnd) {
	table->hnd[sig] = hnd;
}

static inline task_signal_hnd task_signal_table_get(struct task_signal_table *table, int sig) {
	return table->hnd[sig];
}

extern void signal_init(struct task_signal_table *table);
