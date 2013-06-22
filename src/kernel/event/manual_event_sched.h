/**
 * @file
 * @brief
 *
 * @date 22.06.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_EVENT_MANUAL_EVENT_SCHED_H_
#define KERNEL_EVENT_MANUAL_EVENT_SCHED_H_

#include <kernel/sched.h>
#include <kernel/sched/wait_queue.h>

#define MANUAL_EVENT_TIMEOUT_INFINITE SCHED_TIMEOUT_INFINITE

struct manual_event {
	struct wait_queue wait_queue;
	int set;
};

#endif /* KERNEL_EVENT_MANUAL_EVENT_SCHED_H_ */
