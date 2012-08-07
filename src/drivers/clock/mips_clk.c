/**
 * @file
 *
 * @brief MIPS build-in timer
 *
 * @date 04.07.2012
 * @author Anton Bondarev
 */


#include <kernel/time/clock_source.h>
#include <kernel/irq.h>
#include <errno.h>

#include <asm/mipsregs.h>

#include <embox/unit.h>

#include <module/embox/arch/system.h>

#define SYS_CLOCK     OPTION_MODULE_GET(embox__arch__system,NUMBER,core_freq)

#define HZ 1000


//http://stackoverflow.com/questions/4211555/clock-implementation-in-mips

static irq_return_t clock_handler(irq_nr_t irq_nr, void *dev_id) {
	uint32_t count = read_c0_count();
	write_c0_compare(count + 10000);
	return IRQ_HANDLED;
}

static int mips_clock_setup(struct time_dev_conf * conf) {
	uint32_t count = read_c0_count();
	write_c0_compare(count + 10000);

	return ENOERR;
}

static struct time_event_device mips_event_device  = {
		.config = mips_clock_setup,
		.resolution = 1000,
		.name = "mips_clk",
		.irq_nr = MIPS_IRQN_TIMER
};

static struct clock_source mips_clock_source = {
	.name = "mips_clk",
	.event_device = &mips_event_device,
//	.counter_device = &mips_counter_device,
//	.read = clock_source_read /* attach default read function */
};

static int mips_clock_init(void) {
	clock_source_register(&mips_clock_source);

	if (ENOERR != irq_attach((irq_nr_t) MIPS_IRQN_TIMER ,
		(irq_handler_t) &clock_handler, 0, NULL, "mips_clk")) {
	}

	return 0;
}

EMBOX_UNIT_INIT(mips_clock_init);
