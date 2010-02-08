/**
 * @file timers.c
 *
 * @date 19.11.09
 * @author Anton Bondarev
 */

#include "kernel/timers.h"
#include "assert.h"
#include "types.h"
#include "drivers/amba_pnp.h"
#include "common.h"
#include "kernel/irq.h"
#include "autoconf.h"

/* timers clock 1 uSec*/
#define TIMER_SCALER_VAL (CORE_FREQ/1000000 - 1)

typedef struct _TIMERS_STRUCT {
	volatile unsigned int scaler_cnt; /**< 0x00 */
	volatile unsigned int scaler_ld;  /**< 0x04 */
	volatile unsigned int config_reg; /**< 0x08 */
	volatile unsigned int dummy1;     /**< 0x0C */
	volatile unsigned int timer_cnt1; /**< 0x10 */
	volatile unsigned int timer_ld1;  /**< 0x14 */
	volatile unsigned int timer_ctrl1;/**< 0x18 */
	volatile unsigned int dummy2;     /**< 0x1C */
	volatile unsigned int timer_cnt2; /**< 0x20 */
	volatile unsigned int timer_ld2;  /**< 0x24 */
	volatile unsigned int timer_ctrl2;/**< 0x28 */
} TIMERS_STRUCT;

static TIMERS_STRUCT * dev_regs = NULL;

#define ASSERT_INIT_DONE() assert_not_null(dev_regs)

#ifndef SIMULATION_TRG
static AMBA_DEV amba_dev;
#endif

#if 0
void platform_timers_off(void) {
	REG_STORE(dev_regs->timer_ctrl1, 0x0);
	REG_STORE(dev_regs->timer_ctrl2, 0x0); /* disable */
}
#endif

#ifndef SIMULATION_TRG
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
}
#endif

int timers_ctrl_init(IRQ_HANDLER irq_handler) {
	uint8_t irq;
	if (dev_regs) {
		return -1;
	}
#ifndef SIMULATION_TRG
	TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_TIMER);
	dev_regs = (TIMERS_STRUCT *) amba_dev.bar[0].start;
	amba_dev.show_info = show_module_info;
	irq = amba_dev.dev_info.irq;
#else
	dev_regs = (TIMERS_STRUCT *) TIMERS_BASE;
	irq = TIMERS_IRQ;
#endif
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

	return request_irq(irq, irq_handler, 0, "apb_timer", NULL);
}
