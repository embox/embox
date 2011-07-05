/**
 * @file
 *
 * @date 02.09.09
 * @author Andrey Baboshin
 */

#ifndef TIMER_H_
#define TIMER_H_

#include __impl_x(kernel/timer/timer_impl.h)

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
 * Set 'handle' timer with 'id' identity for executing every 'ticks' ms.
 *
 * @param id timer identifier
 * @param ticks assignable time
 * @param handle the function to be executed
 *
 * @return whether the timer is set
 * @retval 1 if the timer is set
 * @retval 0 if the timer isn't set
 */
extern int set_timer(timer_t id, uint32_t ticks, TIMER_FUNC handle);

/**
 * Shut down timer with 'id' identity
 *
 * @param id timer identifier
 */
extern void close_timer(timer_t id);

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
 *  TIMER_FLAG_DETACHED
 *   -- (use only with TIMER_FLAG_ALLOCATE) timer ptr will deallocated by timer routine. You can't use timer ptr after run set_timer with this flag.
 *  TIMER_FLAG_ALLOCATE
 *   -- timer ptr will be allocated inside set_timer routine. (timer ptr argument for set_timer can be any random ptr or NULL)
 *
 * @return value is NULL when set_timer failed and another when successed.
 *
 */

//timer_t* set_timer( timer_t** ptr,


#endif /* TIMER_H_ */

