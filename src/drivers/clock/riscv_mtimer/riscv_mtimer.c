/**
 * @file
 *
 * @brief RISC-V build-in timer
 *
 * @date 12.12.2019
 * @author Anastasia Nizharadze
 */

#include <stddef.h>
#include <stdint.h>

#include <asm/csr.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <hal/cpu.h>
#include <riscv/clint.h>

#define RTC_FREQ OPTION_GET(NUMBER, rtc_freq)

#define NSEC_PER_CYCLE (NSEC_PER_SEC / RTC_FREQ)
#define CYCLE_PER_NSEC (RTC_FREQ / NSEC_PER_SEC)
#define CYCLE_PER_TICK (RTC_FREQ / JIFFIES_PERIOD)

static uint64_t prev_mtimecmp[NCPU];
static uint64_t curr_mtimecmp[NCPU];

static cycle_t riscv_mtimer_get_cycles(struct clock_source *cs) {
	return (cycle_t)(clint_get_time() - prev_mtimecmp[cpu_get_id()]);
}

static uint64_t riscv_mtimer_get_time(struct clock_source *cs) {
#if NSEC_PER_CYCLE >= 1
	return clint_get_time() * NSEC_PER_CYCLE;
#else
	return clint_get_time() / CYCLE_PER_NSEC;
#endif
}

static irq_return_t riscv_mtimer_irq_handler(unsigned int irq_nr, void *data) {
	unsigned int cpu = cpu_get_id();
	uint64_t now = clint_get_time();
	unsigned int ticks;

	if ((curr_mtimecmp[cpu] == 0) || (now < curr_mtimecmp[cpu])) {
		/* The first interrupt this core takes, or one taken on a deadline
		 * that has not come: either way the difference below would be a
		 * wrap, not an elapsed time. */
		curr_mtimecmp[cpu] = now;
		ticks = 1;
	}
	else {
		ticks = (now - curr_mtimecmp[cpu]) / CYCLE_PER_TICK + 1;
	}

	prev_mtimecmp[cpu] = curr_mtimecmp[cpu];
	curr_mtimecmp[cpu] += ticks * CYCLE_PER_TICK;

	clint_set_timer(curr_mtimecmp[cpu]);

	clock_handle_ticks(data, ticks);

	return IRQ_HANDLED;
}

static int riscv_mtimer_set_periodic(struct clock_source *cs) {
	unsigned int irq;

	clint_set_timer(0);

	irq = irqctrl_set_level(RISCV_IRQ_TIMER, 1);
	irq_attach(irq, riscv_mtimer_irq_handler, 0, cs, NULL);

	return 0;
}

static struct time_counter_device riscv_mtimer_counter_device = {
    .get_cycles = riscv_mtimer_get_cycles,
    .get_time = riscv_mtimer_get_time,
    .cycle_hz = RTC_FREQ,
};

static struct time_event_device riscv_mtimer_event_device = {
    .set_periodic = riscv_mtimer_set_periodic,
};

CLOCK_SOURCE_DEF(riscv_mtimer, NULL, NULL, &riscv_mtimer_event_device,
    &riscv_mtimer_counter_device);
