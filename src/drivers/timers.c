#include "types.h"
#include "leon_config.h"
#include "memory_map.h"
#include "timers.h"
#include "irq.h"


#ifdef LEON3
typedef struct _TIMERS_STRUCT
{
    volatile unsigned int scaler_cnt;		/* 0x00 */
    volatile unsigned int scaler_ld;		/* 0x04 */
    volatile unsigned int config_reg;		/* 0x08 */
    volatile unsigned int dummy1;		/* 0x0C */
    volatile unsigned int timer_cnt1;		/* 0x10 */
    volatile unsigned int timer_ld1;		/* 0x14 */
    volatile unsigned int timer_ctrl1;		/* 0x18 */
    volatile unsigned int dummy2;		/* 0x1C */
    volatile unsigned int timer_cnt2;		/* 0x20 */
    volatile unsigned int timer_ld2;		/* 0x24 */
    volatile unsigned int timer_ctrl2;		/* 0x28 */
}TIMERS_STRUCT;
#endif
#ifdef LEON2
typedef struct _TIMERS_STRUCT
{
	volatile unsigned int timer_cnt1;		// 0x40
	volatile unsigned int timer_ld1;
	volatile unsigned int timer_ctrl1;
	volatile unsigned int wdog;
	volatile unsigned int timer_cnt2;		//0x50
	volatile unsigned int timer_ld2;
	volatile unsigned int timer_ctrl2;
	volatile unsigned int dummy8;
	volatile unsigned int scaler_cnt;		//0x60
	volatile unsigned int scaler_ld;
	volatile unsigned int dummy9;
	volatile unsigned int dummy10;
}TIMERS_STRUCT;
#endif
static TIMERS_STRUCT *const timers = (TIMERS_STRUCT *)(TIMERS_BASE);



SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];




volatile static UINT32 cnt_ms_sleep;//for sleep function
volatile static UINT32 cnt_sys_time;//quantity ms after start system



BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle)
{
	UINT32 i;
	for (i = 0; i < sizeof (sys_timers)/sizeof(sys_timers[0]); i ++)
	{
		if(!sys_timers[i].f_enable)
		{
			sys_timers[i].handle = handle;
			sys_timers[i].load = ticks;
			sys_timers[i].cnt = ticks;
			sys_timers[i].id = id;
			sys_timers[i].f_enable = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

void close_timer (UINT32 id)
{
	UINT32 i;
	for (i = 0; i < sizeof (sys_timers)/sizeof(sys_timers[0]); i ++)
	{
		if(id == sys_timers[i].id)
		{
			sys_timers[i].f_enable = FALSE;
			sys_timers[i].handle = NULL;
		}
	}
}

static void inc_sys_timers ()
{
	UINT32 i;
	for (i = 0; i < sizeof (sys_timers)/sizeof(sys_timers[0]); i ++)
	{
		if(sys_timers[i].f_enable)
		{
			if (!(sys_timers[i].cnt --))
			{
				sys_timers[i].cnt = sys_timers[i].load;
				if (sys_timers[i].handle)
					sys_timers[i].handle (sys_timers[i].id);
			}
		}
	}
}

static void irq_func_tmr2 ()
{
	cnt_ms_sleep ++;
	cnt_sys_time ++;
	inc_sys_timers();
}


void timers_init()
{
#ifndef RELEASE
	int i;
	for (i = 0; i < sizeof (sys_timers)/sizeof(sys_timers[0]); i ++)
		sys_timers[i].f_enable=FALSE;

	timers->scaler_ld = TIMER_SCALER_VAL;
	timers->scaler_cnt = 0;
	timers->timer_cnt1 = 0;
	timers->timer_cnt2 = 0;

	timers->timer_ld1 = 0x002710;//0x00030d40;
	timers->timer_ld2 = 0x002710;//0x00989680;
	timers->timer_ctrl1 = 3;
	timers->timer_ctrl2 = 3;
	irq_set_handler(IRQ_Timer2, irq_func_tmr2);
//	irq_set_handler(IRQ_Timer1, irq_func_tmr1);
	cnt_sys_time = 0;
#endif

}


void sleep (int ms)
{
	cnt_ms_sleep = 0;
	while (ms > cnt_ms_sleep);
}

UINT32 get_sys_time ()
{
	return cnt_sys_time;
}
