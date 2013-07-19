/**
 * @file
 * @brief
 *
 * @date 14.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_EVENT_NOSCHED_H_
#define KERNEL_EVENT_NOSCHED_H_

#include <errno.h>

#define EVENT_TIMEOUT_INFINITE -1

struct event {

};

#define EVENT_WAIT(event, cond_expr, timeout) ENOERR

#define EVENT_WAIT_OR_INTR(event, cond_expr, timeout) ENOERR

#endif /* KERNEL_EVENT_NOSCHED_H_ */
