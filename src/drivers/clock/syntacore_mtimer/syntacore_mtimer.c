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

#include <asm/csr.h>
#include <asm/interrupts.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define BASE_ADDR UINTMAX_C(OPTION_GET(NUMBER, base_addr))

#define COUNT_OFFSET (RTC_CLOCK / JIFFIES_PERIOD)

#define RTC_CLOCK OPTION_GET(NUMBER, rtc_freq)

#define SCR1_TIMER_ENABLE_S 0
#define SCR1_TIMER_ENABLE_M (1 << SCR1_TIMER_ENABLE_S)

#define SCR1_TIMER_CLKSRC_S          1
#define SCR1_TIMER_CLKSRC_M          (1 << SCR1_TIMER_CLKSRC_S)
#define SCR1_TIMER_CLKSRC_INTERNAL_M (0 << SCR1_TIMER_CLKSRC_S)
#define SCR1_TIMER_CLKSRC_RTC_M      (1 << SCR1_TIMER_CLKSRC_S)

struct syntacore_mtimer_regs {
	volatile uint32_t TIMER_CTRL;
	volatile uint32_t TIMER_DIV;
	/* CLINT timer */
	volatile uint32_t MTIME;
	volatile uint32_t MTIMEH;
	volatile uint32_t MTIMECMP;
	volatile uint32_t MTIMECMPH;
};

static struct syntacore_mtimer_regs *SCR1_TIMER = (void *)(uintptr_t)BASE_ADDR;

static int syntacore_mtimer_clock_handler(unsigned int irq_nr, void *dev_id) {
	//REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	REG64_STORE(&SCR1_TIMER->MTIMECMP,
	    REG64_LOAD(&SCR1_TIMER->MTIME) + COUNT_OFFSET);

	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static int syntacore_mtimer_clock_setup(struct clock_source *cs) {
	//REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);

	REG64_STORE(&SCR1_TIMER->MTIMECMP,
	    REG64_LOAD(&SCR1_TIMER->MTIME) + COUNT_OFFSET);

	return ENOERR;
}

static int syntacore_mtimer_init(struct clock_source *cs) {
	SCR1_TIMER->TIMER_DIV = 0;
	SCR1_TIMER->TIMER_CTRL |= SCR1_TIMER_ENABLE_M;
	//SCR1_TIMER->TIMER_CTRL &= ~SCR1_TIMER_ENABLE_M;

	enable_timer_interrupts();

	return ENOERR;
}

static struct time_event_device syntacore_mtimer_event_device = {
    .set_periodic = syntacore_mtimer_clock_setup,
    .name = "syntacore_mtimer",
    .irq_nr = RISCV_IRQ_TIMER,
};

CLOCK_SOURCE_DEF(syntacore_mtimer, syntacore_mtimer_init, NULL,
    &syntacore_mtimer_event_device, NULL);

RISCV_TIMER_IRQ_DEF(syntacore_mtimer_clock_handler,
    &CLOCK_SOURCE_NAME(syntacore_mtimer));
