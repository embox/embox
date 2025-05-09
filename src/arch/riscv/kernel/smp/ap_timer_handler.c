/**
 * @file
 * @brief
 *
 * @date 29.08.2024
 * @author Zeng Zixian
 */

#include <hal/clock.h>
#include <asm/regs.h>
#include <framework/mod/options.h>

#include <kernel/lthread/lthread.h>
#include <kernel/time/timer.h>
#include <kernel/time/time.h>

#include <drivers/interrupt/riscv_clint/riscv_clint.h>
#include <module/embox/driver/clock/riscv_clk.h>

#define RTC_CLOCK OPTION_MODULE_GET(embox__driver__clock__riscv_clk, NUMBER, rtc_freq)
#define COUNT_OFFSET  (RTC_CLOCK / JIFFIES_PERIOD)

/* from jiffies.c */
extern struct clock_source *cs_jiffies;

/* Called with timer interrupt off */
void __riscv_ap_timer_handler(void *dev_id) {
#if SMODE
	register uintptr_t a7 asm("a7") = (uintptr_t)(OPENSBI_TIMER);
	register uintptr_t a6 asm("a6") = (uintptr_t)(0);
	register uintptr_t a0 asm("a0") = 0;
	asm volatile("rdtime a0");
	a0 = a0 + COUNT_OFFSET;
	(void)a7;
	(void)a6;
	asm volatile("ecall");
#else
	set_timecmp(get_current_time() + COUNT_OFFSET, cpu_get_id());
#endif
	if (dev_id == cs_jiffies) {
		jiffies_update(1);
	} else {
		clock_handle_ticks(dev_id, 1);
	}
}
