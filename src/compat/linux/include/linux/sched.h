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


#endif /* __LINUX_SCHED_H_ */
