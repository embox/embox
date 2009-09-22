/**
 * \file timers_helper.c
 */

#include "asm/types.h"
#include "conio/conio.h"
#include "common.h"
#include "kernel/irq.h"
#include "kernel/timers.h"
#include "string.h"

typedef struct _SYS_TMR {
        volatile BOOL f_enable;
        volatile UINT32 id;
        volatile UINT32 load;
        volatile UINT32 cnt;
        volatile TIMER_FUNC handle;
} SYS_TMR;

static SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];

void set_sys_timer_enable (int num, BOOL val) {
        sys_timers[num].f_enable = val;
}

BOOL set_timer (UINT32 id, UINT32 ticks, TIMER_FUNC handle) {
	UINT32 i;
	for (i = 0; i < array_len (sys_timers); i++) {
		if (!sys_timers[i].f_enable) {
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

void close_timer (UINT32 id) {
	UINT32 i;
	for (i = 0; i < array_len (sys_timers); i++) {
		if (id == sys_timers[i].id) {
			sys_timers[i].f_enable = FALSE;
			sys_timers[i].handle = NULL;
		}
	}
}

/*static */void inc_sys_timers () {
	UINT32 i;
	for (i = 0; i < array_len (sys_timers); i++) {
		if (sys_timers[i].f_enable) {
			if (!(sys_timers[i].cnt--)) {
				sys_timers[i].cnt = sys_timers[i].load;
				if (sys_timers[i].handle)
					sys_timers[i].handle (sys_timers[i].id);
			}
		}
	}
}

//TODO now save only one context
static SYS_TMR save_timers_buffer[MAX_QUANTITY_SYS_TIMERS][MAX_SAVE_CONTEXT];
static int save_context_number = 0;

int timers_context_save () {
	int context_number = 0;
	memcpy (save_timers_buffer[context_number], sys_timers, sizeof (sys_timers));
	return context_number;
}

int timers_context_restore (int context_number) {
	memcpy (sys_timers, save_timers_buffer[context_number], sizeof (sys_timers));
	return context_number;
}

void timers_off () {
	platform_timers_off ();
//	dev_regs->timer_ctrl1 = 0x0;
//	dev_regs->timer_ctrl2 = 0x0;	//disable
}
