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
#include <kernel/sched/waitq.h>

#define MANUAL_EVENT_TIMEOUT_INFINITE SCHED_TIMEOUT_INFINITE

struct manual_event {
	struct waitq waitq;
	int set;
};

#endif /* KERNEL_EVENT_MANUAL_EVENT_SCHED_H_ */
