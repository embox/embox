/**
 * @file
 *
 * @brief MIPS build-in timer
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */

#include <errno.h>

#include <asm/mipsregs.h>
#include <hal/clock.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <embox/unit.h>

#define HZ 1000
#define COUNT_OFFSET (SYS_CLOCK / HZ)

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	mips_write_c0_compare(COUNT_OFFSET); /* this lowers irq line */
	mips_write_c0_count(0);
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static cycle_t mips_clk_read(struct clock_source *cs) {
	uint32_t cnt = mips_read_c0_count();
	return cnt;
}

static int mips_clock_setup(struct clock_source *cs) {
	mips_write_c0_compare(COUNT_OFFSET);
	mips_write_c0_count(0);
	return ENOERR;
}

static struct time_event_device mips_event_device  = {
	.set_periodic = mips_clock_setup,
	.name = "mips_clk",
	.irq_nr = MIPS_IRQN_TIMER
};

static struct time_counter_device mips_counter_device  = {
	.read = mips_clk_read,
	.cycle_hz = SYS_CLOCK
};

static int mips_clock_init(struct clock_source *cs) {
	int err;

	err = irq_attach(MIPS_IRQN_TIMER, clock_handler, 0, cs, "mips_clk");
	if (err) {
		return err;
	}

	return 0;
}

CLOCK_SOURCE_DEF(mips_clk, mips_clock_init, NULL,
	&mips_event_device, &mips_counter_device);
