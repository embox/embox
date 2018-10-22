/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    29.05.2014
 */

#ifndef THREAD_WAIT_H_
#define THREAD_WAIT_H_

struct thread_wait;
struct sys_timer;

extern void thread_wait_init(struct thread_wait *tw);

extern void thread_wait_add(struct thread_wait *tw, struct sys_timer *tmr);

extern void thread_wait_del(struct thread_wait *tw, struct sys_timer *tmr);

extern void thread_wait_deinit(struct thread_wait *tw);

#include <module/embox/kernel/thread/thread_wait.h>

#endif /* THREAD_WAIT_H_ */

