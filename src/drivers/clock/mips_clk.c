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
	clock_tick_handler(irq_nr, dev_id);
	return IRQ_HANDLED;
}

static int mips_clock_setup(struct time_dev_conf * conf) {
	mips_write_c0_compare(COUNT_OFFSET);
	mips_write_c0_count(0);
	return ENOERR;
}

static struct time_event_device mips_event_device  = {
	.config = mips_clock_setup,
	.event_hz = 1000,
	.name = "mips_clk",
	.irq_nr = MIPS_IRQN_TIMER
};

static struct clock_source mips_clock_source = {
	.name = "mips_clk",
	.event_device = &mips_event_device,
	.read = clock_source_read /* attach default read function */
};

static int mips_clock_init(void) {
	int err;

	err = clock_source_register(&mips_clock_source);
	if (err) {
		return err;
	}

	err = irq_attach(MIPS_IRQN_TIMER, clock_handler, 0, &mips_clock_source, "mips_clk");
	if (err) {
		return err;
	}

	return 0;
}

EMBOX_UNIT_INIT(mips_clock_init);
