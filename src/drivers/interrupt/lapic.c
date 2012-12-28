/**
 * @file
 * @brief
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <string.h>
#include <unistd.h>
#include <types.h>

#include <asm/msr.h>
#include <asm/ap.h>

#include "lapic.h"

EMBOX_UNIT_INIT(unit_init);

#define lapic_write_icr1(val)	lapic_write(LAPIC_ICR1, val)
#define lapic_write_icr2(val)	lapic_write(LAPIC_ICR2, val)

void lapic_send_startup_ipi(uint32_t apic_id, uint32_t trampoline) {
	uint32_t val;

	val = 0;
	val |= apic_id << 24;
	lapic_write_icr2(val);

	val = 0;
	val |= (1 << 14) | (6 << 8);
	val |= ((trampoline >> 12) & 0xFF);
	lapic_write_icr1(val);
}

void lapic_send_init_ipi(uint32_t apic_id) {
	uint32_t val;

	val = 0;
	val |= apic_id << 24;
	lapic_write_icr2(val);

	val = 0;
	val |= (1 << 14) | (5 << 8);
	lapic_write_icr1(val);
}

static inline void lapic_enable_in_msr(void) {
	uint32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);
	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);
}

int lapic_enable(void) {
	uint32_t val;

	lapic_enable_in_msr();

	/* Clear error state register */
	lapic_errstatus();

#if 1
	lapic_write(LAPIC_LVT_PCR, 4<<8);
#endif

    /* Set the spurious interrupt vector register */
	val = lapic_read(LAPIC_SIVR);
	val |= 0x100;
	lapic_write(LAPIC_SIVR, val);

	return 0;
}

static int unit_init(void) {
	return lapic_enable();
}
