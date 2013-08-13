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
#include <stdint.h>

#include <asm/msr.h>
#include <asm/ap.h>

#include <hal/arch.h>

#include <kernel/cpu/cpu.h>
#include <kernel/thread.h>
#include <kernel/spinlock.h>

#include <module/embox/driver/interrupt/lapic.h>
//TODO WTF
#include <module/embox/kernel/thread/core.h>

EMBOX_UNIT_INIT(unit_init);

#define TRAMPOLINE_ADDR 0x20000UL

static spinlock_t startup_lock = SPIN_UNLOCKED;

char AP_STACK[NCPU][THREAD_STACK_SIZE] __attribute__((aligned(THREAD_STACK_SIZE)));

extern struct thread *thread_init_self(void *stack, size_t stack_sz,
		sched_priority_t priority);

void startup_ap(void) {
	extern void idt_load(void);

	struct thread *bootstrap;

	idt_load();

	lapic_enable();

	bootstrap = thread_init_self(__ap_sp - THREAD_STACK_SIZE, THREAD_STACK_SIZE,
			THREAD_PRIORITY_MIN);

	cpu_init(cpu_get_id(), bootstrap);

	__asm__ __volatile__ ("sti");

	spin_unlock(&startup_lock);

	while(1) {
		arch_idle();
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
	/* Setting up stack and boot */
	__ap_sp = AP_STACK[cpu_id] + THREAD_STACK_SIZE;

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
    for (int i = 0; i < NCPU; i++) {
    	if (i == lapic_id()) {
    		continue;
    	}

    	spin_lock(&startup_lock);
    	cpu_start(i);

    	/* For synchronization */
    	spin_lock(&startup_lock);
    	spin_unlock(&startup_lock);
    }

    return 0;
}

void smp_send_resched(int cpu_id) {
	lapic_send_ipi(0x50, cpu_id, LAPIC_IPI_DEST);
}

void resched(void) {
	extern void sched_post_switch(void);

	lapic_send_eoi();

	sched_post_switch();
}

