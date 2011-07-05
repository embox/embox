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
extern int set_timer(uint32_t id, uint32_t ticks, TIMER_FUNC handle);

/**
 * Shut down timer with 'id' identity
 *
 * @param id timer identifier
 */
extern int close_timer(uint32_t id);

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

/**
 * todo:
 */

extern int timer_set(timer_ptr *timer, uint32_t time, uint32_t flags, TIMER_F functor, void *args);

extern int timer_close(timer_ptr *timer);

//int timer_deatach( timer_t** ptr );

/* TODO it's static func*/
extern uint32_t get_free_timer_id(void);
extern struct event * get_timer_event_by_id(uint32_t id);


#endif /* TIMER_H_ */

