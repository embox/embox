/**
 * @file
 * @brief
 *
 * @date 25.07.2013
 * @author Andrey Gazukin
 */

#ifndef __LINUX_SCHED_H_
#define __LINUX_SCHED_H_

#include <sched.h>

#define cond_resched() do { } while(0)
#define signal_pending(x) (0)

inline void flush_signals (void *task) {}

#endif /* __LINUX_SCHED_H_ */
