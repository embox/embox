/**
 * @file
 *
 * @date 02.09.09
 * @author Andrey Baboshin
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <sys/types.h>

#include __impl_x(kernel/timer/timer_impl.h)

#define TIMER_POOL_SZ 20 /**<system timers quantity */

struct sys_tmr;
typedef struct sys_tmr sys_tmr_t;
typedef sys_tmr_t *sys_tmr_ptr;

typedef void (*TIMER_FUNC)(sys_tmr_ptr timer, void *param);

/**
 * timer type is timer_t
 */

/**
 * Timer functor type is TIMER_FUNC
 */

/**
 * Initialization of the timers subsystem.
 * int timer_init(void);
 * We don't need it, since it calling by UNIT framework and must be
 * used only by the framework
 */

/**
 * Set 'handle' timer for executing every 'ticks' ms.
 *
 * @param pointer to sys_tmr_ptr
 * @param ticks assignable time
 * @param handle the function to be executed
 *
 * @return whether the timer is set
 * @retval 0 if the timer is set
 * @retval non-0 if the timer isn't set
 */
extern int set_timer(sys_tmr_ptr *ptimer, uint32_t ticks, TIMER_FUNC handle, void *param);

/**
 * Shut down timer with system_tmr_ptr identity
 *
 * @param id timer identifier
 */
extern int close_timer(sys_tmr_ptr *ptimer);

/**
 * Save timers context. Now saving only one context.
 */
extern int timers_context_save(void);

/**
 * Restore context by it number.
 */
extern int timers_context_restore(int context_number);

/**
 * Shutdown timers subsystem.
 */
extern void timers_off(void);

extern uint32_t cnt_system_time(void);

/**
 *
 * flags for timer:

 *  TIMER_FLAG_PERIODICAL
 *   -- timer must be periodical or not
 	 when timer is non-periodical it will be deallocated after execution.

 *
 * @return value is -ERROR when set_timer failed and 0 when successed.
 *
 */

//extern int timer_set(timer_ptr *timer, uint32_t time, uint32_t flags, TIMER_F functor, void *args);
//extern int timer_close(timer_ptr *timer);

#endif /* TIMER_H_ */
