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
#include <stdint.h>

#include <drivers/common/memory.h>

#include "lapic.h"

#ifdef LAPIC_REGS_X86_H_
#include <asm/msr.h>
#include <asm/ap.h>
#endif

#include <kernel/panic.h>

#define lapic_read_icr1()    lapic_read(LAPIC_ICR1)
#define lapic_read_icr2()    lapic_read(LAPIC_ICR2)

#define lapic_write_icr1(val)   lapic_write(LAPIC_ICR1, val)
#define lapic_write_icr2(val)   lapic_write(LAPIC_ICR2, val)

#define LAPIC_ICR_DM_INIT            (5 <<  8)
#define LAPIC_ICR_DM_STARTUP         (6 <<  8)
#define LAPIC_ICR_DELIVERY_PENDING	 (1 << 12)
#define LAPIC_ICR_INT_ASSERT         (1 << 14)
#define LAPIC_ICR_INIT_LEVELTRIG     (1 << 15)
#define LAPIC_ICR_DEST_FIELD         (0 << 18)
#define LAPIC_ICR_DEST_SELF          (1 << 18)
#define LAPIC_ICR_DEST_ALL           (2 << 18)
#define LAPIC_ICR_DEST_ALL_BUT_SELF  (3 << 18)

#define	LAPIC_DISABLE    0x10000
#define	LAPIC_SW_ENABLE  0x100
#define	LAPIC_CPUFOCUS   0x200
#define	LAPIC_NMI        (4<<8)
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

	val = (trampoline >> 12) & 0xFF; /* Vector Field */
	val |= LAPIC_ICR_DM_STARTUP; /* Delivery Mode: Start Up */
	val |= LAPIC_ICR_DEST_FIELD; /* Destination Shorthand: field specified*/
	lapic_write_icr1(val);

	udelay(1000);

	val = (trampoline >> 12) & 0xFF; /* Vector Field */
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

	/* I don't know why this masks. */
	icr1 = lapic_read_icr1() & 0xFFF0F800;
	icr2 = lapic_read_icr2() & 0xFFFFFF;

	switch (type) {
		case LAPIC_IPI_DEST:
			lapic_write_icr2(icr2 |	(cpu << 24));
			lapic_write_icr1(icr1 |	LAPIC_ICR_DEST_FIELD | vector);
			break;
		case LAPIC_IPI_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	LAPIC_ICR_DEST_SELF | vector);
			break;
		case LAPIC_IPI_TO_ALL_BUT_SELF:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	LAPIC_ICR_DEST_ALL_BUT_SELF | vector);
			break;
		case LAPIC_IPI_TO_ALL:
			lapic_write_icr2(icr2);
			lapic_write_icr1(icr1 |	LAPIC_ICR_DEST_ALL | vector);
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

#if 0
	/* Clear error state register */
	lapic_errstatus();

	lapic_write(LAPIC_DFR, 0xFFFFFFFF);

	val = lapic_read(LAPIC_LDR);
	val &= 0x00FFFFFF;
	val |= 1;
	lapic_write(LAPIC_LDR, val);
	lapic_write(LAPIC_LVT_TR, LAPIC_DISABLE);
	lapic_write(LAPIC_LVT_PCR, LAPIC_NMI);
	lapic_write(LAPIC_LVT_LINT0, LAPIC_DISABLE);
	lapic_write(LAPIC_LVT_LINT1, LAPIC_DISABLE);
	lapic_write(LAPIC_TASKPRIOR, 0);
#endif

    /* Set the spurious interrupt vector register */
	val = lapic_read(LAPIC_SIVR);
	val |= 0x100;
	lapic_write(LAPIC_SIVR, val);

	return 0;
}

PERIPH_MEMORY_DEFINE(local_apic, LOCAL_APIC_DEF_ADDR, 0x1000);
