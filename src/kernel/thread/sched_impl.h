/**
 * @file
 * @brief TODO documentation for sched_impl.h -- Alina
 *
 * @date 23.03. 2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_SCHED_IMPL_H_
#define KERNEL_THREAD_SCHED_IMPL_H_

#include "types.h"
#include <kernel/thread/sched_logic.h>

inline static struct thread *scheduler_current(void) {
	return _scheduler_current();
}

#endif /* KERNEL_THREAD_SCHED_IMPL_H_ */
