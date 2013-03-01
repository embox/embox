/**
 * @file
 * @brief Definitions of signal table, types and some constants
 *
 * @author  Alexander Kalmuk
 * @date    22.10.2012
 */

#ifndef SRC_INCLUDE_KERNEL_TASK_SIGNAL_COMMON_H_
#define SRC_INCLUDE_KERNEL_TASK_SIGNAL_COMMON_H_

#include <stdint.h>
#include <kernel/task.h>
#include <util/dlist.h>

union sigval {
	int sival_int;
	void *sival_ptr;
};

typedef void(*global_sig_hnd_t)(void);
/* Handler for standard signal */
typedef void (*task_signal_hnd_t)(int sig);
/* Handler for realtime signal */
typedef void (*task_rtsignal_hnd_t)(int sig, union sigval value);

#define TASK_SIGNAL_MAX_N 16

#define TASK_SIGRTMIN     17
#define TASK_SIGRTMAX     20
#define TASK_RTSIG_CNT  (TASK_SIGRTMAX - TASK_SIGRTMIN + 1)
#define TASK_RTSIG_CNT_PER_TYPE 3

struct task_signal_table {
	uint32_t sig_mask;
	char last_sig;
	task_signal_hnd_t hnd[TASK_SIGNAL_MAX_N];
	task_rtsignal_hnd_t rt_hnd[TASK_RTSIG_CNT];
	struct dlist_head rtsig_data[TASK_RTSIG_CNT];
};

#define SIGNAL_HANDLE_ENTRY(func) \
		extern global_sig_hnd_t __signal_handlers_array[]; \
		ARRAY_SPREAD_ADD(__signal_handlers_array, func)

#endif /* SRC_INCLUDE_KERNEL_TASK_SIGNAL_COMMON_H_ */
