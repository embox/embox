/**
 * \file timers.h
 */

#ifndef _TIMERS_H_
#define _TIMERS_H_

#ifndef __ASSEMBLER__

#define MAX_QUANTITY_SYS_TIMERS   0x20
#define DEFAULT_SLEEP_COUNTER     3470
#define MAX_SAVE_CONTEXT          2


/**
 * Initialization of timers subsystem
 */
int timers_init ();

typedef void (*TIMER_FUNC) (UINT32 id);

/**
 * Set 'handle' timer with 'id' identity for executing every 'ticks' ms.
 */
BOOL set_timer (UINT32 id, UINT32 ticks, TIMER_FUNC handle);

/**
 * Shut down timer with 'id' identity
 */
void close_timer (UINT32 id);

/**
 * Sleep a number of 'ms' millisecond.
 */
void sleep (unsigned int ms);

//void calibrate_sleep ();

/**
 * Quantity of ms after starting the system
 */
UINT32 get_sys_time ();

/**
 * Save timers context. Now saving only one context.
 */
int timers_context_save ();

/**
 * Restore context by it number.
 */
int timers_context_restore (int context_number);

/**
 * Shutdown timers subsystem.
 */
void timers_off ();

#endif //__ASSEMBLER

#endif //_TIMERS_H_
