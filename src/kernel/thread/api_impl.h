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
	list_for_each_entry(thread_ptr, __extension__ ({   \
				extern struct list_head __thread_list; \
				&__thread_list;                        \
			}), thread_link)                           \

inline static struct thread *thread_get_by_id(__thread_id_t id) {
	struct thread *thread;

	__thread_foreach(thread) {
		if (thread->id == id) {
			return thread;
		}
	}

	return NULL;
}

inline static struct thread *thread_current(void) {
	return sched_current();
}

#endif /* KERNEL_THREAD_API_IMPL_H_ */
