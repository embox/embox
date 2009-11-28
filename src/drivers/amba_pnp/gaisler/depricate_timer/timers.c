/**
 * \file timers.c
 */

#include "kernel/timers.h"

#include "types.h"
#include "drivers/amba_pnp.h"
#include "common.h"
#include "kernel/irq.h"
#include "autoconf.h"

#define TIMER_SCALER_VAL (CORE_FREQ/1000000 - 1)

typedef struct _TIMERS_STRUCT {
	volatile unsigned int scaler_cnt; /**< 0x00 */
	volatile unsigned int scaler_ld; /**< 0x04 */
	volatile unsigned int config_reg; /**< 0x08 */
	volatile unsigned int dummy1; /**< 0x0C */
	volatile unsigned int timer_cnt1; /**< 0x10 */
	volatile unsigned int timer_ld1; /**< 0x14 */
	volatile unsigned int timer_ctrl1; /**< 0x18 */
	volatile unsigned int dummy2; /**< 0x1C */
	volatile unsigned int timer_cnt2; /**< 0x20 */
	volatile unsigned int timer_ld2; /**< 0x24 */
	volatile unsigned int timer_ctrl2; /**< 0x28 */
} TIMERS_STRUCT;

static TIMERS_STRUCT * dev_regs = NULL;

#define ASSERT_INIT_DONE() assert_not_null(dev_regs)

static AMBA_DEV amba_dev;

static volatile uint32_t cnt_ms_sleep; /**< for sleep function */
static volatile uint32_t cnt_sys_time; /**< quantity ms after start system */

volatile static uint32_t sleep_cnt_const = DEFAULT_SLEEP_COUNTER; /**< for sleep function (loop-based) */

void platform_timers_off() {
	REG_STORE(dev_regs->timer_ctrl1, 0x0);
	REG_STORE(dev_regs->timer_ctrl2, 0x0); /* disable */
}

static void irq_func_tmr_1mS() {
	unsigned int irq = local_irq_save();
	cnt_ms_sleep++;
	cnt_sys_time++;
	inc_sys_timers();
	local_irq_restore(irq);
}

//TODO this must be in kernel part
void sleep(volatile unsigned int ms) {
	unsigned int irq = local_irq_save();
	cnt_ms_sleep = 0;
	local_irq_restore(irq);

	while (cnt_ms_sleep < ms) {
	}
}

uint32_t get_ms_sleep() {
	return cnt_ms_sleep;
}

static void show_module_info(AMBA_DEV * dev) {
	TRACE ("*** GAISLER timers ***\n");
	TRACE ("regs:\n");
	TRACE ("\tcnt1 0x%X\n", REG_LOAD(dev_regs->timer_cnt1));
	TRACE ("\tld1 0x%X\n", REG_LOAD(dev_regs->timer_ld1));
	TRACE ("\tctrl1 0x%X\n", REG_LOAD(dev_regs->timer_ctrl1));

	TRACE ("\tcnt2 0x%X\n", REG_LOAD(dev_regs->timer_cnt2));
	TRACE ("\tld2 0x%X\n", REG_LOAD(dev_regs->timer_ld2));
	TRACE ("\tctrl2 0x%X\n", REG_LOAD(dev_regs->timer_ctrl2));

	TRACE ("\tscaler_cnt 0x%X\n", REG_LOAD(dev_regs->scaler_cnt));
	TRACE ("\tscaler_ld 0x%X\n", REG_LOAD(dev_regs->scaler_ld));
	TRACE ("system resources\n");
	TRACE ("\tcnt_ms_sleep 0x%X\n", cnt_ms_sleep);
	TRACE ("\tcnt_sys_time 0x%X\n", cnt_sys_time);
}

int timers_init() {
	if (dev_regs) {
		return -1;
	}
	int i;
	IRQ_INFO irq_info;
#ifndef SIMULATION_TRG
	for (i = 0; i < /*array_len (sys_timers)*/MAX_QUANTITY_SYS_TIMERS; i++)
		set_sys_timer_enable(i, false);
	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_TIMER);
#else
	amba_dev.bar[0].start = TIMERS_BASE;
	amba_dev.dev_info.irq = TIMERS_IRQ;
#endif

	dev_regs = (TIMERS_STRUCT *) amba_dev.bar[0].start;
	REG_STORE(dev_regs->timer_ctrl1, 0x0);
	REG_STORE(dev_regs->timer_ctrl2, 0x0); /**< disable */

	REG_STORE(dev_regs->scaler_ld, TIMER_SCALER_VAL);
	REG_STORE(dev_regs->scaler_cnt, 0);
	REG_STORE(dev_regs->timer_cnt1, 0);
	REG_STORE(dev_regs->timer_cnt2, 0);

	REG_STORE(dev_regs->timer_ld1, 1000000 / 1000 - 1);
	REG_STORE(dev_regs->timer_ld2, 0);
	REG_STORE(dev_regs->timer_ctrl1, 0xf);
	REG_STORE(dev_regs->timer_ctrl2, 0x0); /**< disable */

	irq_info.enabled = true;
	irq_info.handler = irq_func_tmr_1mS;
	irq_info.irq_num = amba_dev.dev_info.irq;
	irq_set_info(&irq_info);
	//irq_set_handler(amba_dev.dev_info.irq, irq_func_tmr_1mS);

	cnt_sys_time = 0;

	return 0;
}

