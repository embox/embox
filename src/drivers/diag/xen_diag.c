#include <drivers/diag.h>
#include <string.h>
#include <util/macro.h>

#include <stdint.h>
#include <xen/xen.h>
#include <hypercall-x86_32.h>
#include <xen/event_channel.h>
#include <xen/io/console.h>
#include <xen/sched.h>
#include <barrier.h>

static evtchn_port_t console_evt;
extern char _text;
struct xencons_interface * console;

static void diag_xen_init(const struct diag *diag, start_info_t * start_info) {
	console = (struct xencons_interface*)
		((machine_to_phys_mapping[start_info->console.domU.mfn] << 12)
		 +
		((unsigned long)&_text));
	console_evt = start_info->console.domU.evtchn;	
}

static void diag_xen_putc(const struct diag *diag, char ch) {
	struct evtchn_send event;
	event.port = console_evt;
	/* Wait for the back end to clear enough space in the buffer */
	XENCONS_RING_IDX data;
	
	do
	{
		data = console->out_prod - console->out_cons;
		HYPERVISOR_event_channel_op(EVTCHNOP_send, &event);
		mb();
	} while (data >= sizeof(console->out));
	
	/* Copy the byte */
	int ring_index = MASK_XENCONS_IDX(console->out_prod, console->out);
	console->out[ring_index] = ch;
	/* Ensure that the data really is in the ring before continuing */
	wmb();
	/* Increment input and output pointers */
	console->out_prod++;
		
	HYPERVISOR_event_channel_op(EVTCHNOP_send, &event);
}

DIAG_OPS_DECLARE(
	.init = diag_xen_init,
	.putc = diag_xen_putc,
);
