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

static inline uint32_t lapic_id(void) {
	return lapic_read(LAPIC_ID) >> 24;
}

static inline uint32_t lapic_errstatus(void)
{
	lapic_write(LAPIC_ESR, 0);
	return lapic_read(LAPIC_ESR);
}

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

void startup_ap(void) {
	while (1) {
		__asm__ __volatile__ ("hlt");
	}
}

static inline void lapic_enable_in_msr(void) {
	uint32_t msr_hi, msr_lo;

	ia32_msr_read(IA32_APIC_BASE, &msr_hi, &msr_lo);
	msr_lo |= (1 << IA32_APIC_BASE_ENABLE_BIT);
	ia32_msr_write(IA32_APIC_BASE, msr_hi, msr_lo);
}

#define TRAMPOLINE_ADDR 0x20000UL

static inline void init_trampoline(void) {
	/* Initialize gdt */
	memcpy(__ap_gdt, gdt, sizeof(gdt));
	gdt_set_gdtr(&__ap_gdtr, __ap_gdt);

	memcpy((void *) TRAMPOLINE_ADDR, &__ap_trampoline,
			(uint32_t) &__ap_trampoline_end - (uint32_t) &__ap_trampoline);
}

static inline void cpu_start(int cpu_id) {
	lapic_send_init_ipi(cpu_id);

	for (int i = 0; i < 20000; i++) {
		__asm__ __volatile__ ("nop");
	}

	lapic_send_init_ipi(cpu_id);

	for (int i = 0; i < 20000; i++) {
		__asm__ __volatile__ ("nop");
	}

	lapic_send_startup_ipi(cpu_id, TRAMPOLINE_ADDR);
}

void lapic_enable(void)
{
	uint32_t val;

	lapic_enable_in_msr();

	/* Clear error state register */
	lapic_errstatus();

    /* Set the spurious interrupt vector register */
	val = lapic_read(LAPIC_SIVR);
	val |= 0x100;
	lapic_write(LAPIC_SIVR, val);

	/* Initialize trampoline for the APs */
	init_trampoline();

    for (int i = 0; i <= 1; i++) {
    	if (i == lapic_id()) {
    		continue;
    	}

    	cpu_start(i);
    }

    usleep(5000);
}

static int unit_init(void) {
	lapic_enable();

	return 0;
}
