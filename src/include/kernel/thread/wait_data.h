/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_THREAD_WAIT_DATA_H_
#define KERNEL_THREAD_WAIT_DATA_H_

#include <kernel/work.h>

struct thread;

struct wait_data {
	struct work work;

	int result;

	void (*on_notified)(struct thread *, void *);
	void *data;
};


#endif /* KERNEL_THREAD_WAIT_DATA_H_ */
