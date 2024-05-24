/**
 * @file
 * @brief MIPS build-in timer
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

#include <stdint.h>

#include <asm/mipsregs.h>
#include <framework/mod/options.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#define CYC_PER_TICK       OPTION_GET(NUMBER, cyc_per_tick)

#define TIMER_IRQ_INTERVAL (SYS_CLOCK / (CYC_PER_TICK * JIFFIES_PERIOD))

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	mips_write_c0_compare(TIMER_IRQ_INTERVAL); /* this lowers irq line */
	mips_write_c0_count(0);

	clock_tick_handler(dev_id);

	return IRQ_HANDLED;
}

static cycle_t mips_clk_read(struct clock_source *cs) {
	return (cycle_t)mips_read_c0_count();
}

static int mips_clock_setup(struct clock_source *cs) {
	mips_write_c0_compare(TIMER_IRQ_INTERVAL);
	mips_write_c0_count(0);

	return 0;
}

static struct time_event_device mips_event_device = {
    .set_periodic = mips_clock_setup,
    .name = "mips_clk",
    .irq_nr = MIPS_IRQN_TIMER,
};

static struct time_counter_device mips_counter_device = {
    .read = mips_clk_read,
    .cycle_hz = SYS_CLOCK,
};

static int mips_clock_init(struct clock_source *cs) {
	return irq_attach(MIPS_IRQN_TIMER, clock_handler, 0, cs, "mips_clk");
}

CLOCK_SOURCE_DEF(mips_clk, mips_clock_init, NULL, &mips_event_device,
    &mips_counter_device);
