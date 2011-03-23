/**
 * @file
 * @brief Threads internal implementation.
 *
 * @date 24.02.2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_API_IMPL_H_
#define KERNEL_THREAD_API_IMPL_H_

#include <lib/list.h>
#include <kernel/thread/sched.h>

#include "types.h"

#define __THREAD_POOL_SZ 0x100

#define __thread_foreach(thread_ptr) \
	array_foreach_ptr(thread_ptr, __extension__ ({     \
				extern struct thread __thread_pool[];  \
				__thread_pool;                         \
			}), __THREAD_POOL_SZ)                      \
		if (!thread_ptr->exist) ; else

inline static struct thread *thread_get_by_id(__thread_id_t id) {
	extern struct thread __thread_pool[];
	struct thread *thread = __thread_pool + id;

	if (!(0 <= id && id < __THREAD_POOL_SZ)) {
		return NULL;
	}

	return thread->exist ? thread : NULL;
}

inline static struct thread *thread_current(void) {
	return sched_current();
}

#endif /* KERNEL_THREAD_API_IMPL_H_ */
