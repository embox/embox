/**
 * @file
 * @brief
 *
 * @date 14.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_EVENT_NOSCHED_H_
#define KERNEL_EVENT_NOSCHED_H_

#define EVENT_TIMEOUT_INFINITE -1

struct event {

};

#define EVENT_WAIT(event, cond_expr, timeout) \
	do { (void) event; } while (0)

#endif /* KERNEL_EVENT_NOSCHED_H_ */
