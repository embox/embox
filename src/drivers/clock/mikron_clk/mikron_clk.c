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

#include <asm/interrupts.h>
#include <asm/regs.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define COUNT_OFFSET  (RTC_CLOCK / JIFFIES_PERIOD)
#define RTC_CLOCK     OPTION_GET(NUMBER, rtc_freq)

#define MTIME         OPTION_GET(NUMBER, base_mtime)
#define MTIMECMP      OPTION_GET(NUMBER, base_mtimecmp)

struct mikron_clk_regs {
	uint32_t TIMER_CTRL;
	uint32_t TIMER_DIV;
	uint32_t MTIME_REG;
	uint32_t MTIMEH;
	uint32_t MTIMECMP_REG;
	uint32_t MTIMECMPH;
};

static int clock_handler(unsigned int irq_nr, void *dev_id) {
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static int riscv_clock_setup(struct clock_source *cs) {
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	return ENOERR;
}

static int mikron_clk_init(struct clock_source *cs) {
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	enable_timer_interrupts();

	return ENOERR;
}

static struct time_event_device riscv_event_device = {
    .set_periodic = riscv_clock_setup,
    .name = "mikron_clk",
    .irq_nr = IRQ_TIMER,
};

CLOCK_SOURCE_DEF(mikron_clk, mikron_clk_init, NULL, &riscv_event_device, NULL);

RISCV_TIMER_IRQ_DEF(clock_handler, &CLOCK_SOURCE_NAME(mikron_clk));
