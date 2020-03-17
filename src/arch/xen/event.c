#include <stdint.h>
#include <xen/xen.h>

#include <barrier.h>
#include <traps.h>

#if defined(__i386__)
#include <xen_hypercall-x86_32.h>
#elif defined(__x86_64__)
#include <xen_hypercall-x86_64.h>
#else
#error "Unsupported architecture"
#endif

#include <assert.h>
#include <kernel/irq.h>

#define NUM_CHANNELS (1024)

//x86 only
//Set bit 'bit' in bitfield 'field'
#define SET_BIT(bit,field) __asm__ __volatile__ ("lock btsl %1,%0":"=m"(field):"Ir"(bit):"memory" );
#define CLEAR_BIT(field, bit) __asm__ __volatile__ ("lock btrl %1,%0":"=m" ((field)):"Ir"((bit)):"memory")

/* Locations in the bootstrapping code */
extern shared_info_t xen_shared_info;
void hypervisor_callback(void);
void failsafe_callback(void);

static inline unsigned long int first_bit(unsigned long int word)
{
#if defined (__i386__)
	__asm__("bsfl %1,%0"
		:"=r" (word)
		:"rm" (word));
#elif defined (__x86_64__)
	__asm__("bsfq %1,%0"
		:"=r" (word)
		:"rm" (word));
#else
#error "Unsupported architecture"
#endif
	return word;
}


static inline unsigned long int xchg(unsigned long int * old, unsigned long int value)
{
#if defined (__i386__)
	__asm__("xchgl %0,%1"
		:"=r" (value)
		:"m" (*old), "0"(value)
		:"memory");
#elif defined (__x86_64__)
	__asm__("xchgq %0,%1"
		:"=r" (value)
		:"m" (*old), "0"(value)
		:"memory");
#else
#error "Unsupported architecture"
#endif
	return value;
}

static void handle(int irq) {
	assert(!critical_inside(CRITICAL_IRQ_LOCK));

	irqctrl_disable(irq);
	irqctrl_eoi(irq);
	critical_enter(CRITICAL_IRQ_HANDLER);
	{
		ipl_enable();

		irq_dispatch(irq);

		ipl_disable();

	}
	irqctrl_enable(irq);
	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

/* Dispatch events to the correct handlers */
void do_hypervisor_callback(struct pt_regs *regs)
{
	unsigned int pending_selector;
	unsigned int next_event_offset;
	vcpu_info_t *vcpu = &xen_shared_info.vcpu_info[0];
	/* Make sure we don't lose the edge on new events... */
	vcpu->evtchn_upcall_pending = 0;
	/* Set the pending selector to 0 and get the old value atomically */
	pending_selector = xchg(&vcpu->evtchn_pending_sel, 0);
	while(pending_selector != 0)
	{
		/* Get the first bit of the selector and clear it */
		next_event_offset = first_bit(pending_selector);
		pending_selector &= ~(1 << next_event_offset);
		unsigned int event;

		/* While there are events pending on unmasked channels */
		while(( event =
		(xen_shared_info.evtchn_pending[pending_selector] 
		 & 
		~xen_shared_info.evtchn_mask[pending_selector]))
		 != 0)
		{
			/* Find the first waiting event */
			unsigned int event_offset = first_bit(event);

			/* Combine the two offsets to get the port */
			evtchn_port_t port = (pending_selector << 5) + event_offset;
			/* Handler the event */
			handle(port);
		}
	}
}
