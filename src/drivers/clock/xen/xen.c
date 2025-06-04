/**
 * @file
 *
 * @date
 * @author
 */

#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <xen/event_channel.h>
#include <xen_hypercall-x86_32.h>

#include <embox/unit.h>

#include <math.h>
#include <time.h>
#include <xen/xen.h>

extern shared_info_t xen_shared_info;

static int integratorcp_clock_setup(struct clock_source *cs) {
	return 0;
}

static struct time_event_device xen_event_device = {
	.name = "xen event device",
	.set_periodic = integratorcp_clock_setup,
	.irq_nr = -1
};

static cycle_t xen_tcd_get_cycles(struct clock_source *cs) {
	return 0;
}

static struct time_counter_device xen_tcd = {
	.cycle_hz = 1000,
	.get_cycles = xen_tcd_get_cycles,
};

static uint64_t system_time;

static uint64_t xen_time(void) {
	return xen_shared_info.vcpu_info[0].time.system_time;
}

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	uint64_t time = xen_time();

	const int n = (time - system_time) / NSEC_PER_MSEC;
	for (int i = 0; i < n; i++) {
		clock_tick_handler(dev_id);
	}

	return IRQ_HANDLED;
}

static int xen_clock_init(struct clock_source *cs) {

	evtchn_bind_virq_t op;

	op.virq = VIRQ_TIMER;
	op.vcpu = 0;
	if (HYPERVISOR_event_channel_op(EVTCHNOP_bind_virq, &op) != 0) {
		panic("Error has happened during timer initialization.\n");
	}
	xen_event_device.irq_nr = op.port;

	system_time = xen_time();

	return irq_attach(op.port, clock_handler, 0, cs, "xen clock irq");
}

CLOCK_SOURCE_DEF(xen,  xen_clock_init, NULL,
	&xen_event_device, &xen_tcd);
