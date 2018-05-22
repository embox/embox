/**
 * @file
 *
 * @data
 * @author
 */

#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <xen/event_channel.h>
#include <xen_hypercall-x86_32.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(xen_clock_init);

static int integratorcp_clock_setup(struct time_dev_conf * conf) {
	return 0;
}

static struct time_event_device xen_event_device = {
	.name = "xen event device",
	.config = integratorcp_clock_setup,
	.event_hz = 1000,
	.irq_nr = -1
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
	for (int i = 0; i < 10; i++) {
		clock_tick_handler(irq_nr, dev_id);
	}
	return IRQ_HANDLED;
}


static int xen_clock_init(void) {

	clock_source_register(&xen_cs);
	evtchn_bind_virq_t op;

	op.virq = VIRQ_TIMER;
	op.vcpu = 0;
	if (HYPERVISOR_event_channel_op(EVTCHNOP_bind_virq, &op) != 0) {
		panic("Error has happened during timer initialization.\n");
	}
	xen_event_device.irq_nr = op.port;

	return irq_attach(op.port, clock_handler, 0, &xen_cs, "xen clock irq");
}
