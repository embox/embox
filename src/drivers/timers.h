#ifndef _TIMERS_H_
#define _TIMERS_H_

int timers_init ();
typedef void (*TIMER_FUNC)(UINT32 id);

BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle);
void close_timer (UINT32 id);

void sleep_1ms_irq (int ms);
inline void sleep (int ms);
void calibrate_sleep ();
UINT32 get_sys_time ();

int timers_context_save ();
int timers_context_restore (int context_number);

#endif //_TIMERS_H_

