/**
 * @file
 *
 * @data
 * @author
 */

#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/irq.h>

#include <embox/unit.h>

// FIXME
#define CLOCK_IRQ 0

EMBOX_UNIT_INIT(xen_clock_init);

static int integratorcp_clock_setup(struct time_dev_conf * conf) {
	return 0;
}

static struct time_event_device xen_event_device = {
	.name = "xen event device",
	.config = integratorcp_clock_setup,
	.event_hz = 1000,
	.irq_nr = CLOCK_IRQ
};

static cycle_t xen_tcd_read(void) {
	return 0;
}

static struct time_counter_device xen_tcd = {
	.cycle_hz = 1000,
	.read = xen_tcd_read,
};

static struct clock_source xen_cs = {
	.name = "xen clock source",
	.event_device = &xen_event_device,
	.counter_device = &xen_tcd,
	.read = clock_source_read /* attach default read function */
};

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(irq_nr, dev_id);
	return IRQ_HANDLED;
}

static int xen_clock_init(void) {
	clock_source_register(&xen_cs);

	return irq_attach(CLOCK_IRQ, clock_handler, 0, &xen_cs, "xen clock irq");
}
