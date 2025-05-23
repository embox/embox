/**
 * @file
 *
 * @brief RISC-V build-in timer
 *
 * @date 12.12.2019
 * @author Anastasia Nizharadze
 */

#include <errno.h>
#include <stdint.h>

#include <asm/regs.h>
#include <asm/interrupts.h>
#include <hal/cpu.h>
#include <hal/reg.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/time.h>
#include <kernel/time/clock_source.h>
#include <framework/mod/options.h>
#include <drivers/interrupt/riscv_clint/riscv_clint.h>

#define COUNT_OFFSET  (RTC_CLOCK / JIFFIES_PERIOD)
#define RTC_CLOCK     OPTION_GET(NUMBER, rtc_freq)

static int clock_handler(unsigned int irq_nr, void *dev_id) {
	set_timecmp(get_current_time() + COUNT_OFFSET, cpu_get_id());
	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static int riscv_clock_setup(struct clock_source *cs) {
	set_timecmp(get_current_time() + COUNT_OFFSET, cpu_get_id());
	enable_timer_interrupts();

	return ENOERR;
}

static struct time_event_device riscv_event_device = {
    .set_periodic = riscv_clock_setup,
    .name = "riscv_timer_clint",
    .irq_nr = IRQ_TIMER,
};

CLOCK_SOURCE_DEF(riscv_clk, NULL, NULL, &riscv_event_device, NULL);

RISCV_TIMER_IRQ_DEF(clock_handler, &CLOCK_SOURCE_NAME(riscv_clk));
