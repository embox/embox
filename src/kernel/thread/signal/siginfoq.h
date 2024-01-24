/**
 * @file
 * @brief RT signals.
 *
 * @date 07.10.2013
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_SIGRT_H_
#define KERNEL_THREAD_SIGRT_H_

#include <lib/libds/slist.h>

struct siginfoq {
	struct slist       queue;
	struct slist_link *last;
};

#endif /* KERNEL_THREAD_SIGRT_H_ */
