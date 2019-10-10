/**
 * @file
 * @brief
 *
 * @date 17.03.2012
 * @author Andrey Golikov
 * @author Anton Kozlov
 */

#include <drivers/diag.h>

#include <stdint.h>
#include <xen/xen.h>
#include <xen/io/console.h>
#include <xen_hypercall-x86_32.h>

#include <xen/sched.h>
#include <xen_barrier.h>
#include <xen/event.h>

static evtchn_port_t console_evt;
extern char _text_vma;
struct xencons_interface * console;

static int diag_xen_init(const struct diag *diag) {
	extern start_info_t *xen_start_info_global;
	console = (struct xencons_interface*)
		((machine_to_phys_mapping[xen_start_info_global->console.domU.mfn] << 12)
		 +
		((unsigned long)&_text_vma));
	console_evt = xen_start_info_global->console.domU.evtchn;
	/* TODO: Set up the event channel */
	return 0;
}

static void diag_xen_putc(const struct diag *diag, char ch) {
	struct evtchn_send event;
	event.port = console_evt;
	char message[1];
	message[0] = ch;
	{
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
		console->out[ring_index] = *message;
		/* Ensure that the data really is in the ring before continuing */
		wmb();
		/* Increment input and output pointers */
		console->out_prod++;
	}
	HYPERVISOR_event_channel_op(EVTCHNOP_send, &event);
}

DIAG_OPS_DEF(
	.init = diag_xen_init,
	.putc = diag_xen_putc,
);
