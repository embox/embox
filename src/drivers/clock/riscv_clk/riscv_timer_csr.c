/**
 * @file
 *
 * @brief RISC-V build-in timer
 *
 * @date 29.04.2025
 * @author Zixian Zeng
 */

#include <errno.h>
#include <stdint.h>

#include <asm/interrupts.h>
#include <asm/regs.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <drivers/interrupt/riscv_clint/riscv_clint.h>

#define COUNT_OFFSET  (RTC_CLOCK / JIFFIES_PERIOD)
#define RTC_CLOCK     OPTION_GET(NUMBER, rtc_freq)

static uint64_t get_current_time64(void) {
#if __riscv_xlen == 32
        uint32_t hi, lo;

        do {
                hi = read_csr(TIMEH_REG);
                lo = read_csr(TIME_REG);
        } while (hi != read_csr(TIMEH_REG));

        return ((uint64_t)hi << 32) | lo;
#else /* __riscv_xlen == 64 */
        return read_csr(TIME_REG);
#endif
}

static int clock_handler(unsigned int irq_nr, void *dev_id) {
	clint_set_mtimecmp(get_current_time64() + COUNT_OFFSET, cpu_get_id());
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

uint64_t clint_get_mtimecmp(int);
static int riscv_clock_setup(struct clock_source *cs) {
	clint_set_mtimecmp(get_current_time64() + COUNT_OFFSET, cpu_get_id());
	enable_timer_interrupts();
	return ENOERR;
}

static struct time_event_device riscv_event_device = {
    .set_periodic = riscv_clock_setup,
    .name = "riscv_timer_csr",
    .irq_nr = IRQ_TIMER,
};

CLOCK_SOURCE_DEF(riscv_clk, NULL, NULL, &riscv_event_device, NULL);

RISCV_TIMER_IRQ_DEF(clock_handler, &CLOCK_SOURCE_NAME(riscv_clk));
