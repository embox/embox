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

#define BASE_ADDR    OPTION_GET(NUMBER, base_addr)

#define COUNT_OFFSET (RTC_CLOCK / JIFFIES_PERIOD)

#define RTC_CLOCK    OPTION_GET(NUMBER, rtc_freq)

#define SCR1_TIMER_ENABLE_S             0
#define SCR1_TIMER_ENABLE_M             (1 << SCR1_TIMER_ENABLE_S)

#define SCR1_TIMER_CLKSRC_S             1
#define SCR1_TIMER_CLKSRC_M             (1 << SCR1_TIMER_CLKSRC_S)
#define SCR1_TIMER_CLKSRC_INTERNAL_M    (0 << SCR1_TIMER_CLKSRC_S)
#define SCR1_TIMER_CLKSRC_RTC_M         (1 << SCR1_TIMER_CLKSRC_S)

struct mikron_clk_regs {
	volatile uint32_t TIMER_CTRL;
	volatile uint32_t TIMER_DIV;
	volatile uint32_t MTIME;
	volatile uint32_t MTIMEH;
	volatile uint32_t MTIMECMP;
	volatile uint32_t MTIMECMPH;
};

static struct mikron_clk_regs *SCR1_TIMER = (void*)(uintptr_t)BASE_ADDR;

static int clock_handler(unsigned int irq_nr, void *dev_id) {
	//REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);
	*(unsigned long long *) &SCR1_TIMER->MTIMECMP += COUNT_OFFSET;

	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static int riscv_clock_setup(struct clock_source *cs) {
	//REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	*(unsigned long long *) &SCR1_TIMER->MTIME = 0;
	*(unsigned long long *) &SCR1_TIMER->MTIMECMP = COUNT_OFFSET;
	//SCR1_TIMER->TIMER_CTRL |= SCR1_TIMER_ENABLE_M;

	return ENOERR;
}

static int mikron_clk_init(struct clock_source *cs) {
	//REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);
	SCR1_TIMER->TIMER_DIV = 0;
	*(unsigned long long *) &SCR1_TIMER->MTIME = 0;
	*(unsigned long long *) &SCR1_TIMER->MTIMECMP = COUNT_OFFSET;
	SCR1_TIMER->TIMER_CTRL |= SCR1_TIMER_ENABLE_M;
	//SCR1_TIMER->TIMER_CTRL &= ~SCR1_TIMER_ENABLE_M;
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
