/**
 * @file
 *
 * @brief RISC-V build-in timer
 *
 * @date 12.12.2019
 * @author Anastasia Nizharadze
 */

#include <errno.h>

#include <asm/regs.h>
#include <asm/interrupts.h>

#include <hal/clock.h>
#include <hal/system.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

#define HZ 1000
#define COUNT_OFFSET (RTC_CLOCK / HZ)
#define RTC_CLOCK    OPTION_GET(NUMBER, rtc_freq)

#define MTIME        OPTION_GET(NUMBER, base_mtime)
#define MTIMECMP     OPTION_GET(NUMBER, base_mtimecmp)

#define OPENSBI_TIMER 0x54494D45

static int clock_handler(unsigned int irq_nr, void *dev_id) {
#if SMODE
	register uintptr_t a7 asm ("a7") = (uintptr_t)(OPENSBI_TIMER);
	register uintptr_t a6 asm ("a6") = (uintptr_t)(0);
	register uintptr_t a0 asm ("a0") = 0;
	asm volatile ("rdtime a0");
	a0 = a0 + COUNT_OFFSET;
	(void)a7; (void)a6;
	asm volatile ("ecall");
#else
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);
#endif

	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static int riscv_clock_setup(struct clock_source *cs) {
#if SMODE
	register uintptr_t a7 asm ("a7") = (uintptr_t)(OPENSBI_TIMER);
	register uintptr_t a6 asm ("a6") = (uintptr_t)(0);
	register uintptr_t a0 asm ("a0") = 0;
	asm volatile ("rdtime a0");
	a0 = a0 + COUNT_OFFSET;
	(void)a7; (void)a6;
	asm volatile ("ecall");
#else
	REG64_STORE(MTIMECMP, REG64_LOAD(MTIME) + COUNT_OFFSET);
#endif

	enable_timer_interrupts();

	return ENOERR;
}

static struct time_event_device riscv_event_device  = {
	.set_periodic = riscv_clock_setup,
	.name = "riscv_clk",
	.irq_nr = IRQ_TIMER
};

CLOCK_SOURCE_DEF(riscv_clk, NULL, NULL,
	&riscv_event_device, NULL);

RISCV_TIMER_IRQ_DEF(clock_handler, &CLOCK_SOURCE_NAME(riscv_clk));
