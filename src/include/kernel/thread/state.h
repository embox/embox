/**
 * @file
 * @brief TODO
 *
 * @date 24.04.2011
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_THREAD_STATE_H_
#define KERNEL_THREAD_STATE_H_

#include <stdbool.h>

#include __impl_x(kernel/thread/state_impl.h)

typedef __thread_state_t thread_state_t;

extern thread_state_t thread_state_init(void);

extern bool thread_state_sleeping(thread_state_t state);
extern bool thread_state_suspended(thread_state_t state);
extern bool thread_state_exited(thread_state_t state);
extern bool thread_state_detached(thread_state_t state);
extern bool thread_state_blocked(thread_state_t state);
extern bool thread_state_running(thread_state_t state);
extern bool thread_state_dead(thread_state_t state);

extern bool thread_state_can_sleep(thread_state_t state);
extern bool thread_state_can_wake(thread_state_t state);
extern bool thread_state_can_suspend(thread_state_t state);
extern bool thread_state_can_resume(thread_state_t state);
extern bool thread_state_can_exit(thread_state_t state);
extern bool thread_state_can_detach(thread_state_t state);

extern thread_state_t thread_state_do_sleep(thread_state_t state);
extern thread_state_t thread_state_do_wake(thread_state_t state);
extern thread_state_t thread_state_do_suspend(thread_state_t state);
extern thread_state_t thread_state_do_resume(thread_state_t state);
extern thread_state_t thread_state_do_exit(thread_state_t state);
extern thread_state_t thread_state_do_detach(thread_state_t state);

#endif /* KERNEL_THREAD_STATE_H_ */
