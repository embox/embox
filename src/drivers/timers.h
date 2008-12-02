#ifndef _TIMERS_H_
#define _TIMERS_H_

#define MAX_QUANTITY_SYS_TIMERS 0x20

typedef void (*TIMER_FUNC)(UINT32 id);

typedef struct _SYS_TMR
{
	volatile BOOL f_enable;
	volatile UINT32 id;
	volatile UINT32 load;
	volatile UINT32 cnt;
	volatile TIMER_FUNC handle;
}SYS_TMR;

extern SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];

void timers_init ();

BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle);
void close_timer (UINT32 id);

void sleep (int ms);

#endif //_TIMERS_H_

