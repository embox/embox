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

EMBOX_UNIT_INIT(unit_init);

#define lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#define lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

uint32_t apicid(void) {
	return lapic_read(LAPIC_ID) >> 24;
}

void lapic_send_startup_ipi(uint32_t apic_id, uint32_t trampoline) {
	uint32_t val;

	val = lapic_read(LAPIC_ICR2) & 0xFFFFFF;
	val |= apic_id << 24;
	lapic_write(LAPIC_ICR2, val);

	/* send SIPI */
	val = lapic_read(LAPIC_ICR1) & 0xFFF32000;
	val |= (1 << 14) | (6 << 8);
	val |= ((trampoline >> 12) & 0xFF);
	lapic_write(LAPIC_ICR1, val);

#if 0
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

    lapic_write_icr2(high);
    lapic_write_icr1(low);
#endif
}

#include <stdio.h>

void cpu_trampoline(void) {
	while (1) {
		__asm__ __volatile__("hlt");
	}
}

#include <string.h>

static inline void lapic_enable_in_msr(void) {
	uint32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);
	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);
}

void lapic_enable(void)
{
	uint32_t val;

	lapic_enable_in_msr();

    /* Set the Spourious Interrupt Vector Register bit 8 to start receiving interrupts */
	val = lapic_read(LAPIC_SIVR);
	val |= 0x100 | 0xff;
	val &= ~(1 << 9);
	lapic_write(LAPIC_SIVR, val);

    memcpy((void *) 0x2000, (char *) cpu_trampoline, 512);

    lapic_send_startup_ipi(apicid() + 1, (uint32_t) 0x2000);
}

static int unit_init(void) {
	lapic_enable();

	return 0;
}
