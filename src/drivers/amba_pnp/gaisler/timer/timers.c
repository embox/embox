/**
 * \file timers.c
 */

#include "types.h"
#include "common.h"
#include "leon_config.h"
#include "irq.h"
#include "amba_pnp.h"
#include "timers.h"

static TIMERS_STRUCT * dev_regs = NULL;

#include "amba_drivers_helper.h"
#undef module_init
#define module_init() timers_init()

#define MAX_QUANTITY_SYS_TIMERS 	0x20

typedef struct _SYS_TMR {
	volatile BOOL f_enable;
	volatile UINT32 id;
	volatile UINT32 load;
	volatile UINT32 cnt;
	volatile TIMER_FUNC handle;
} SYS_TMR;

static SYS_TMR sys_timers[MAX_QUANTITY_SYS_TIMERS];

static volatile UINT32 cnt_ms_sleep;      /**< for sleep function */
static volatile UINT32 cnt_sys_time;      /**< quantity ms after start system */

#define DEFAULT_SLEEP_COUNTER	3470
volatile static UINT32 sleep_cnt_const = DEFAULT_SLEEP_COUNTER; /**< for sleep function (loop-based) */

BOOL set_timer(UINT32 id, UINT32 ticks, TIMER_FUNC handle) {
	UINT32 i;
	for (i = 0; i < array_len(sys_timers); i++) {
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

void close_timer(UINT32 id) {
	UINT32 i;
	for (i = 0; i < array_len(sys_timers); i++) {
		if (id == sys_timers[i].id) {
			sys_timers[i].f_enable = FALSE;
			sys_timers[i].handle = NULL;
		}
	}
}

static void inc_sys_timers() {
	UINT32 i;
	for (i = 0; i < array_len(sys_timers); i++) {
		if (sys_timers[i].f_enable) {
			if (!(sys_timers[i].cnt--)) {
				sys_timers[i].cnt = sys_timers[i].load;
				if (sys_timers[i].handle)
					sys_timers[i].handle(sys_timers[i].id);
			}
		}
	}
}

static void irq_func_tmr_1mS() {
    unsigned int irq = __local_irq_save();
    cnt_ms_sleep++;
	cnt_sys_time++;
	inc_sys_timers();
    local_irq_restore(irq);
}

static void show_module_info(AMBA_DEV *dev) {
	TRACE ("*** GAISLER timers ***\n");
	TRACE ("regs:\n");
	TRACE ("\tcnt1 0x%X\n", dev_regs->timer_cnt1);
	TRACE ("\tld1 0x%X\n", dev_regs->timer_ld1);
	TRACE ("\tctrl1 0x%X\n", dev_regs->timer_ctrl1);

	TRACE ("\tcnt2 0x%X\n", dev_regs->timer_cnt2);
	TRACE ("\tld2 0x%X\n", dev_regs->timer_ld2);
	TRACE ("\tctrl2 0x%X\n", dev_regs->timer_ctrl2);

	TRACE ("\tscaler_cnt 0x%X\n", dev_regs->scaler_cnt);
	TRACE ("\tscaler_ld 0x%X\n", dev_regs->scaler_ld);
	TRACE ("system resurses\n");
	TRACE ("\tcnt_ms_sleep 0x%X\n",cnt_ms_sleep);
	TRACE ("\tcnt_sys_time 0x%X\n",cnt_sys_time);
}

int timers_init() {
	int i;
	if (dev_regs) {
		return -1;
	}
	for (i = 0; i < array_len(sys_timers); i++)
		sys_timers[i].f_enable = FALSE;


	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_TIMER);
	amba_dev.show_info = show_module_info;

	dev_regs = (TIMERS_STRUCT *)amba_dev.bar[0].start;
	dev_regs->timer_ctrl1 = 0x0;
	dev_regs->timer_ctrl2 = 0x0; /**< disable */

	dev_regs->scaler_ld = TIMER_SCALER_VAL;
	dev_regs->scaler_cnt = 0;
	dev_regs->timer_cnt1 = 0;
	dev_regs->timer_cnt2 = 0;

	dev_regs->timer_ld1 = 0x002710;
	dev_regs->timer_ld2 = 0;//0x0271000;
	dev_regs->timer_ctrl1 = 0xf;
	dev_regs->timer_ctrl2 = 0x0; /**<disable */
	//dev_regs->config_reg = 0x0000;
	irq_set_handler(amba_dev.dev_info.irq, irq_func_tmr_1mS);

	cnt_sys_time = 0;

	return 0;
}
void temp_wait()
{
    int i;
    for(i=0; i < 50; i ++);
    return;
}

void sleep(volatile unsigned int ms) {
    //static unsigned int tmp;
    unsigned int irq = __local_irq_save();
	cnt_ms_sleep = 0;
	local_irq_restore(irq);

	while (cnt_ms_sleep <= ms)	{
	    //temp_wait();
	}
}
/*
void sleep(int ms) {
	register UINT32 cnt;
	for(; ms; ms--) {
		for (cnt = sleep_cnt_const; cnt; cnt--) {}
	}
}

void calibrate_sleep () {
	int t_prev, dt;
	int cnt=0;

        LOG_DEBUG("Start sleep counter calibration ... ");
	// 1. test for proper work of irq_func_tmr_1mS()
	cnt_ms_sleep = 0;
	for (cnt=0; cnt<100000; cnt++) {}
	if (0 == cnt_ms_sleep) {
		// irq_func_tmr_1mS() not worked.
		// we can't calibrate sleep
		// use nearest experimental-based value for 50Mhz CPU freq
		sleep_cnt_const = DEFAULT_SLEEP_COUNTER;
	        LOG_DEBUG("failed. Assumed default value %d for 50Mhz CPU.\n", sleep_cnt_const);
		return;
	}

	// now calibrate volatile int counter for 1ms loop
	sleep_cnt_const = t_prev = 0;
	dt = 256;
	while (dt > 0) {
		// do time-alignment for timer IRQ
		cnt_ms_sleep = 0;
		while (cnt_ms_sleep < 1) {}
	       	// now cnt_ms_sleep == 1

		// test delay
		sleep(10);

		if (cnt_ms_sleep > 10) {
			sleep_cnt_const = t_prev;
			dt /= 2;
		} else {
			t_prev = sleep_cnt_const;
			sleep_cnt_const += dt;
		}
	}
        LOG_DEBUG("done. Assumed %d for sleep(1ms)\n", sleep_cnt_const);
}

*/
UINT32 get_sys_time() {
	return cnt_sys_time;
}

//TODO now save only one context
#define MAX_SAVE_CONTEXT 2
static SYS_TMR save_timers_buffer[MAX_QUANTITY_SYS_TIMERS][MAX_SAVE_CONTEXT];
static int save_context_number = 0;

int timers_context_save() {
	int context_number = 0;
	memcpy(save_timers_buffer[context_number], sys_timers, sizeof(sys_timers));
	return context_number;
}

int timers_context_restore(int context_number) {
	memcpy(sys_timers, save_timers_buffer[context_number], sizeof(sys_timers));
	return context_number;
}

void timers_off (){
	dev_regs->timer_ctrl1 = 0x0;
	dev_regs->timer_ctrl2 = 0x0;//disable
	show_module_info(&amba_dev);
}

