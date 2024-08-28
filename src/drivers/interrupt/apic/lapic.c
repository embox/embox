/**
 * @file
 * @brief
 *
 * @date 18.12.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <drivers/common/memory.h>

#include "lapic.h"

#ifdef LAPIC_REGS_X86_H_
#include <asm/msr.h>
#include <asm/ap.h>
#endif

#include <kernel/panic.h>
#include <hal/cpu.h>

#define lapic_read_icr1()    lapic_read(LAPIC_ICR1)
#define lapic_read_icr2()    lapic_read(LAPIC_ICR2)

#define lapic_write_icr1(val)   lapic_write(LAPIC_ICR1, val)
#define lapic_write_icr2(val)   lapic_write(LAPIC_ICR2, val)

#define	TMR_PERIODIC     0x20000
#define	TMR_BASEDIV      (1<<20)

static void udelay(int delay) {
	volatile int i;

	for (i = delay * 10; i != 0; i-- );
}

void lapic_send_init_ipi(uint32_t apic_id) {
	uint32_t val;

	val = (apic_id & 0xFF) << 24; /* Destination Field */
	lapic_write_icr2(val);

	/* since target cpu is not init yet, this ipi MUST in physical mode */
	val = LAPIC_ICR_DM_INIT; /* Delivery Mode: INIT */
	val |= LAPIC_ICR_INT_ASSERT; /* Level: Assert */
	val |= LAPIC_ICR_DEST_FIELD; /* Destination Shorthand: field specified*/
	val |= LAPIC_ICR_INIT_LEVELTRIG;/*triger level: set high level*/	
	lapic_write_icr1(val);

	udelay(10000);

	/* sleep until Delivery Status is Pending */
	while (lapic_read_icr1() & LAPIC_ICR_DELIVERY_PENDING);
}

void lapic_send_startup_ipi(uint32_t apic_id, uint32_t trampoline) {
	uint32_t val;

	val = (apic_id & 0xFF) << 24; /* Destination Field */
	lapic_write_icr2(val);

	/* since target cpu is not init yet, this ipi MUST in physical mode */
	val = (trampoline >> 12) & 0xFF; /* Vector Field */
	val |= LAPIC_ICR_DM_FIXED; /* Delivery Mode: FIXED */
	val |= LAPIC_ICR_DM_STARTUP; /* Delivery Mode: Start Up */
	val |= LAPIC_ICR_DEST_FIELD; /* Destination Shorthand: field specified*/
	lapic_write_icr1(val);

	udelay(1000);

	val = (trampoline >> 12) & 0xFF; /* Vector Field */
	val |= LAPIC_ICR_DM_FIXED; /* Delivery Mode: FIXED */
	val |= LAPIC_ICR_DM_STARTUP; /* Delivery Mode: Start Up */
	val |= LAPIC_ICR_DEST_FIELD; /* Destination Shorthand: field specified*/
	lapic_write_icr1(val);

	udelay(1000);

	/* sleep until Delivery Status is Pending */
	while (lapic_read_icr1() & LAPIC_ICR_DELIVERY_PENDING);
}

void lapic_send_ipi(unsigned int vector, unsigned int cpu, int type) {
	uint32_t icr1, icr2;

	while (lapic_read_icr1() & LAPIC_ICR_DELIVERY_PENDING) {
		panic("Not implemented\n");
	}

	/* Clear all ICR state */
	icr1 = icr2 = 0;

	switch (type) {
		case LAPIC_ICR_DEST_FIELD:
			lapic_write_icr2(icr2 |	((0x1<<cpu) << 24));
			lapic_write_icr1(icr1 |	type | LAPIC_ICR_LOGICAL_DEST | vector);
			break;
		case LAPIC_ICR_DEST_SELF:
			/* Do not care about the delivery mode */
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	type | vector);
			break;
		case LAPIC_ICR_DEST_ALL:
			/* Do not care about the delivery mode */
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	type | vector);
			break;
		case LAPIC_ICR_DEST_ALL_BUT_SELF:
			/* Do not care about the delivery mode */
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	type | vector);
			break;
		default:
			panic("Unknown send ipi type request\n");
			break;
	}

}

static inline void lapic_enable_in_msr(void) {
#ifdef LAPIC_REGS_X86_H_ /* Needed only on x86 */
	uint32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_lo, &msr_hi);
	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_lo, msr_hi);
#endif /* LAPIC_REGS_X86_H_ */
}

int lapic_enable(void) {
	uint32_t val;

	lapic_enable_in_msr();

    /**
	 * Current Model is Flat Model of logical destination Mode
	 * 0xF in DFR is Flat Model, 0x0 in DFR is cluster Model
	 * ATTENTION: Flat Model limits the number of CPU to 8
	 */
	val = (0x1 << cpu_get_id()) << 24; /* logical APIC ID is specified in bit */
	lapic_write(LAPIC_LDR, val);
	val = (0xF << 28);
	lapic_write(LAPIC_DFR, val);

    /* Set the spurious interrupt vector register */
	val = lapic_read(LAPIC_SIVR);
	val |= 0x100;
	lapic_write(LAPIC_SIVR, val);

#if 0
	/* Clear error state register */
	lapic_errstatus();

	lapic_write(LAPIC_LVT_TR, LAPIC_DISABLE);
	lapic_write(LAPIC_LVT_PCR, LAPIC_NMI);
	lapic_write(LAPIC_LVT_LINT0, LAPIC_DISABLE);
	lapic_write(LAPIC_LVT_LINT1, LAPIC_DISABLE);
	lapic_write(LAPIC_TASKPRIOR, 0);
#endif

	return 0;
}

PERIPH_MEMORY_DEFINE(local_apic, LOCAL_APIC_DEF_ADDR, 0x1000);
