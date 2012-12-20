/**
 * @file
 * @brief
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <types.h>
#include <asm/msr.h>

#include "apic.h"

#define APIC_INTERRUPT_COMMAND_REGISTER     0x30

EMBOX_UNIT_INIT(unit_init);

#if 0
void sendSIPI(uint32_t apicId, uint32_t addr) {
    uint32_t high = 0;
    uint32_t low  = 0;

    low = (addr >> 12) & 0xFF;
    low |= 6 << 8;                          // Start up IPI
    //icr.DestinationMode = 0;                // Physical
    //icr.DeliveryStatus = 0;                 // Idle
    //icr.Reserved1 = 0;
    low |= 1 << 14;                            // Assert
    //icr.TriggerMode = 0;                    // Edge
    //icr.Reserved2 = 0;
    //icr.DestinationShorthand = 0;           // No shorthand
    //icr.Reserved3 = 0;
    //icr.Reserved4 = 0;
    high = apicId << 24;// Processor to send SIPI to
    //icr.Destination = apicId;               // Processor to send SIPI to

    lapic_write(APIC_INTERRUPT_COMMAND_REGISTER + 1, high);
    lapic_write(APIC_INTERRUPT_COMMAND_REGISTER, low);
}

void newKernel(void) {
	asm ("hlt");
}

#include <string.h>
#endif

static inline void lapic_enable_in_msr(void) {
	uint32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);
	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);
}

void lapic_enable(void)
{
	lapic_enable_in_msr();

    /* Set the Spourious Interrupt Vector Register bit 8 to start receiving interrupts */
	//lapic_write(APIC_SPURIOUS_INTERRUPT_VECTOR, lapic_read(APIC_SPURIOUS_INTERRUPT_VECTOR) | 0x100);

#if 0
    memcpy((void *) 0x2000, (char *)newKernel, 512);
    sendSIPI(1, (uint32_t) 0x2000);
#endif
}

static int unit_init(void) {
	lapic_enable();

	return 0;
}
