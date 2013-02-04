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
#define	APIC_APICID	 0x20
#define	APIC_APICVER	 0x30
#define	APIC_TASKPRIOR	 0x80
#define	APIC_EOI	 0x0B0
#define	APIC_LDR	 0x0D0
#define	APIC_DFR	 0x0E0
#define	APIC_SPURIOUS	 0x0F0
#define	APIC_ESR	 0x280
#define	APIC_ICRL	 0x300
#define	APIC_ICRH	 0x310
#define	APIC_LVT_TMR	 0x320
#define	APIC_LVT_PERF	 0x340
#define	APIC_LVT_LINT0	 0x350
#define	APIC_LVT_LINT1	 0x360
#define	APIC_LVT_ERR	 0x370
#define	APIC_TMRINITCNT	 0x380
#define	APIC_TMRCURRCNT	 0x390
#define	APIC_TMRDIV	 0x3E0
#define	APIC_LAST	 0x38F
#define	APIC_DISABLE	 0x10000
#define	APIC_SW_ENABLE	 0x100
#define	APIC_CPUFOCUS	 0x200
#define	APIC_NMI	 (4<<8)
#define	TMR_PERIODIC	 0x20000
#define	TMR_BASEDIV	 (1<<20)


	lapic_write(LOCAL_APIC_DEF_ADDR+APIC_DFR, 0xFFFFFFFF);

	val = lapic_read(LOCAL_APIC_DEF_ADDR + APIC_LDR);
	val &= 0x00FFFFFF;
	val |= 1;
	lapic_write(LOCAL_APIC_DEF_ADDR+APIC_LDR, val);
	lapic_write(LOCAL_APIC_DEF_ADDR+APIC_LVT_TMR, APIC_DISABLE);
	lapic_write(LOCAL_APIC_DEF_ADDR+APIC_LVT_PERF, APIC_NMI);
	lapic_write(LOCAL_APIC_DEF_ADDR+APIC_LVT_LINT0, APIC_DISABLE);
	lapic_write(LOCAL_APIC_DEF_ADDR+APIC_LVT_LINT1, APIC_DISABLE);
	lapic_write(LOCAL_APIC_DEF_ADDR + APIC_TASKPRIOR, 0);
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
