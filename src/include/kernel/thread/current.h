/**
 * @file
 * @brief
 *
 * @date 13.02.12
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_CURRENT_API_H_
#define KERNEL_THREAD_CURRENT_API_H_

#include <module/embox/kernel/thread/current/api.h>

#define thread_get_current() \
	__thread_get_current()

#define thread_set_current(thread) \
	__thread_set_current(thread)

#endif /* KERNEL_THREAD_CURRENT_API_H_ */
