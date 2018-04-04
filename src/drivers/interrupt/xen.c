/**
 * @file
 * @brief
 *
 * @date
 * @author
 */

#include <stdint.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <embox/unit.h>
#include <kernel/printk.h>
#include <xen_hypercall-x86_32.h>
#include <xen/xen.h>
#include <string.h>

#include <drivers/irqctrl.h>
#include <xen/event.h>

extern shared_info_t xen_shared_info;

EMBOX_UNIT_INIT(xen_int_init);

int port;

static int xen_int_init(void) {
	evtchn_alloc_unbound_t op_sender;
	op_sender.dom = DOMID_SELF;
	op_sender.remote_dom = DOMID_SELF;
	if (HYPERVISOR_event_channel_op (EVTCHNOP_alloc_unbound, &op_sender) != 0)
	{
		return -1;
	}
	port = op_sender.port;
	evtchn_bind_interdomain_t op_recipient;
	op_recipient.remote_dom = DOMID_SELF;
	op_recipient.remote_port = op_sender.port;
	if ( HYPERVISOR_event_channel_op ( EVTCHNOP_bind_interdomain, &op_recipient) != 0 )
	{
		return -1;
	}
	return 0;
}

void irqctrl_enable(unsigned int irq) {
}

void irqctrl_disable(unsigned int irq) {
	printk("in disable\n");
}

void irqctrl_force(unsigned int irq) {
	struct evtchn_send event;
	event.port = port;
	if (HYPERVISOR_event_channel_op(EVTCHNOP_send, &event) != 0) {
		panic("ops");
	};
}

void interrupt_handle(void) {
}
