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

#define CLEAR_BIT(field, bit) __asm__ __volatile__ ("lock btrl %1,%0":"=m" ((field)):"Ir"((bit)):"memory")

extern shared_info_t xen_shared_info;
shared_info_t *shared_info;

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
	evtchn_bind_interdomain_t op_recipient;
	op_recipient.remote_dom = DOMID_SELF;
	op_recipient.remote_port = op_sender.port;
	if ( HYPERVISOR_event_channel_op ( EVTCHNOP_bind_interdomain, &op_recipient) != 0 )
	{
		return -1;
	}
	port = op_recipient.local_port;
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
	shared_info = &xen_shared_info;
	int size = sizeof(xen_ulong_t) * 8;
	//shared_info->evtchn_mask[irq / size] &= ~(1UL << irq % size); //TODO: Need to choose..
	//CLEAR_BIT(shared_info->evtchn_mask[irq / size], irq % size);
	shared_info->evtchn_mask[irq / size] &= 0;
	shared_info->vcpu_info[0].evtchn_upcall_mask = 0; //TODO: Searching for the right bit..

	if (HYPERVISOR_event_channel_op(EVTCHNOP_send, &event) != 0) {
		panic("ops");
	};
}

void interrupt_handle(void) {
	printk("Here is IRQ!");
}
