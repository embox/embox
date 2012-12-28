/**
 * @file
 * @brief
 *
 * @date 24.12.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <string.h>
#include <unistd.h>
#include <types.h>

#include <asm/msr.h>
#include <asm/ap.h>

#include <kernel/cpu.h>

#include <module/embox/driver/interrupt/lapic.h>

EMBOX_UNIT_INIT(unit_init);

#define TRAMPOLINE_ADDR 0x20000UL

void startup_ap(void) {
	extern int lapic_enable(void);
	extern void lapic_timer_init(uint32_t quantum);
	extern void idt_load(void);

	idt_load();

	lapic_enable();
	lapic_timer_init(1);
	__asm__ __volatile__ ("sti");

	while (1) {
		;
	}
}

static inline void init_trampoline(void) {
	/* Initialize gdt */
	memcpy(__ap_gdt, gdt, sizeof(gdt));
	gdt_set_gdtr(&__ap_gdtr, __ap_gdt);

	memcpy((void *) TRAMPOLINE_ADDR, &__ap_trampoline,
			(uint32_t) &__ap_trampoline_end - (uint32_t) &__ap_trampoline);
}

/* TODO: FIX THIS! */
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

static int unit_init(void)
{
	/* Initialize trampoline for the APs */
	init_trampoline();

	/* Start all CPUs */
    for (int i = 0; i <= NCPU - 1; i++) {
    	if (i == lapic_id()) {
    		continue;
    	}

    	cpu_start(i);
    }

    return 0;
}
