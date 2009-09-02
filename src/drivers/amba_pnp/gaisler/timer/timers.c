/**
 * \file timers.c
 */

#include "asm/types.h"
#include "drivers/amba_pnp/amba_pnp.h"
#include "kernel/timers_helper.h"

#include "amba_drivers_helper.h"
#undef module_init
#define module_init() timers_init()

static void
show_module_info (AMBA_DEV * dev)
{
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
    TRACE ("system resources\n");
    TRACE ("\tcnt_ms_sleep 0x%X\n", cnt_ms_sleep);
    TRACE ("\tcnt_sys_time 0x%X\n", cnt_sys_time);
}

int
timers_init ()
{
    int i;
    if (dev_regs)
    {
	return -1;
    }
#ifndef SIMULATION_TRG
    for (i = 0; i < array_len (sys_timers); i++)
	sys_timers[i].f_enable = FALSE;

    TRY_CAPTURE_APB_DEV (&amba_dev, VENDOR_ID_GAISLER, DEV_ID_GAISLER_TIMER);
#else
    amba_dev.bar[0].start = TIMERS_BASE;
    amba_dev.dev_info.irq = TIMERS_IRQ;
#endif

    dev_regs = (TIMERS_STRUCT *) amba_dev.bar[0].start;
    dev_regs->timer_ctrl1 = 0x0;
    dev_regs->timer_ctrl2 = 0x0;     /**< disable */

    dev_regs->scaler_ld = TIMER_SCALER_VAL;
    dev_regs->scaler_cnt = 0;
    dev_regs->timer_cnt1 = 0;
    dev_regs->timer_cnt2 = 0;

    dev_regs->timer_ld1 = 1000000 / 1000 - 1;
    dev_regs->timer_ld2 = 0;
    dev_regs->timer_ctrl1 = 0xf;
    dev_regs->timer_ctrl2 = 0x0;     /**<disable */
    irq_set_handler (amba_dev.dev_info.irq, irq_func_tmr_1mS);

    cnt_sys_time = 0;

    return 0;
}
