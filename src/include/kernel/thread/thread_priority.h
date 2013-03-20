/**
 * @file
 * @brief
 *
 * @date 13.03.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_PRIORITY_H_
#define KERNEL_THREAD_PRIORITY_H_

#define THREAD_PRIORITY_MIN 0   /**< The lowest priority is 0. */
#define THREAD_PRIORITY_MAX 255 /**< The highest priority. */

/** Total amount of valid priorities. */
#define THREAD_PRIORITY_TOTAL \
	(THREAD_PRIORITY_MAX - THREAD_PRIORITY_MIN + 1)

#define THREAD_PRIORITY_NORMAL \
	(THREAD_PRIORITY_MIN + THREAD_PRIORITY_MAX) / 2

#define THREAD_PRIORITY_LOW  \
	(THREAD_PRIORITY_MIN + THREAD_PRIORITY_NORMAL) / 2
#define THREAD_PRIORITY_HIGH \
	(THREAD_PRIORITY_MAX + THREAD_PRIORITY_NORMAL) / 2

#define THREAD_PRIORITY_DEFAULT \
	THREAD_PRIORITY_NORMAL

typedef short __thread_priority_t;

#endif /* KERNEL_THREAD_PRIORITY_H_ */
