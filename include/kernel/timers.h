/**
 * \file timers.h
 */

#ifndef _TIMERS_H_
#define _TIMERS_H_

#ifndef __ASSEMBLER__


/**
 * Initialization of timers subsystem
 */
int timers_ctrl_init();

typedef void (*TIMER_FUNC)(UINT32 id);

/**
 * Set 'handle' timer with 'id' identity for executing every 'ticks' ms.
 */
BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle);

/**
 * Shut down timer with 'id' identity
 */
void close_timer(UINT32 id);


/**
 * Save timers context. Now saving only one context.
 */
int timers_context_save();

/**
 * Restore context by it number.
 */
int timers_context_restore(int context_number);

/**
 * Shutdown timers subsystem.
 */
void timers_off();

#endif //__ASSEMBLER
#endif //_TIMERS_H_
