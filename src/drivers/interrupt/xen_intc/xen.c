/**
 * @file
 * @brief
 *
 * @date
 * @author
 */

#include <string.h>
#include <lib/libds/bitmap.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <xen_hypercall-x86_32.h>
#include <embox/unit.h>

#include <drivers/irqctrl.h>

static struct {
	int port1;
	int port2;
} test_ports[2];

extern shared_info_t xen_shared_info;

static int make_test_events(void) {
	int res;
	for (int i = 0; i < ARRAY_SIZE(test_ports); ++i) {
		evtchn_alloc_unbound_t op_alloc = {
			.dom = DOMID_SELF,
			.remote_dom = DOMID_SELF,
		};
		if ((res = HYPERVISOR_event_channel_op(EVTCHNOP_alloc_unbound, &op_alloc))) {
			return res;
		}
		evtchn_bind_interdomain_t op_bind = {
			.remote_dom = DOMID_SELF,
			.remote_port = op_alloc.port,
		};
		if ((res = HYPERVISOR_event_channel_op(EVTCHNOP_bind_interdomain, &op_bind))) {
			return res;
		}

		test_ports[i].port1 = op_alloc.port;
		test_ports[i].port2 = op_bind.local_port;
	}
	return 0;
}

static int init_event_mask(void) {
	memset(xen_shared_info.evtchn_mask, 0xff, sizeof(xen_shared_info.evtchn_mask));
	memset(xen_shared_info.evtchn_pending, 0, sizeof(xen_shared_info.evtchn_pending));
	return 0;
}

static int xen_intc_init(void) {
	int res;

	if ((res = init_event_mask())) {
		return res;
	}

	if ((res = make_test_events())) {
		return res;
	}

	return 0;
}

void irqctrl_enable(unsigned int irq) {
	bitmap_clear_bit(xen_shared_info.evtchn_mask, irq);
}

void irqctrl_disable(unsigned int irq) {
	bitmap_set_bit(xen_shared_info.evtchn_mask, irq);
}

static int find_other_test_port(int port) {
	for (int i = 0; i < ARRAY_SIZE(test_ports); ++i) {
		if (test_ports[i].port1 == port) {
			return test_ports[i].port2;
		}
		if (test_ports[i].port2 == port) {
			return test_ports[i].port1;
		}
	}
	return -1;
}

void irqctrl_force(unsigned int irq) {
	int res;

	const int port = find_other_test_port(irq);
	if (port < 0) {
		printk("%s: unable to force %d\n", __func__, irq);
		return;
	}

	struct evtchn_send ev_send = {
		.port = port,
	};
	if ((res = HYPERVISOR_event_channel_op(EVTCHNOP_send, &ev_send))) {
		printk("%s: send %d failed: %d\n", __func__, irq, res);
	}
}

void irqctrl_eoi(unsigned int irq) {
	bitmap_clear_bit(xen_shared_info.evtchn_pending, irq);
}

IRQCTRL_DEF(xen_intc, xen_intc_init);
