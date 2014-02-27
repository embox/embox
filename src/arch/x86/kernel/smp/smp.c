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
#include <kernel/thread/thread_register.h>
#include <kernel/task.h>
#include <kernel/spinlock.h>


#include <module/embox/driver/interrupt/lapic.h>
#include <module/embox/kernel/thread/core.h>

#define THREAD_STACK_SIZE OPTION_MODULE_GET(embox__kernel__thread__core, \
			NUMBER,thread_stack_size)

EMBOX_UNIT_INIT(unit_init);

#define TRAMPOLINE_ADDR 0x20000UL
extern struct thread *thread_init_self(void *stack, size_t stack_sz,
		sched_priority_t priority);
extern void idt_load(void);

static char ap_stack[NCPU][THREAD_STACK_SIZE]
		__attribute__((aligned(THREAD_STACK_SIZE)));
static int ap_ack;
static spinlock_t startup_lock = SPIN_STATIC_UNLOCKED;

void startup_ap(void) {
	struct thread *bs_idle;
	int self_id = lapic_id();

	__spin_lock(&startup_lock);

	idt_load();
	lapic_enable();

	bs_idle = thread_init_self(__ap_sp - THREAD_STACK_SIZE, THREAD_STACK_SIZE,
			THREAD_PRIORITY_MIN);

	thread_register(task_kernel_task(), bs_idle);

	cpu_init(self_id, bs_idle);
	ap_ack = 1;

	__spin_unlock(&startup_lock);
	ipl_enable();

	while (1)
		arch_idle();
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
	__ap_sp = ap_stack[cpu_id] + THREAD_STACK_SIZE;

	lapic_send_init_ipi(cpu_id);

	for (int i = 0; i < 20000; i++) {
		__asm__ __volatile__ ("nop");
	}

	lapic_send_init_ipi(cpu_id);

	for (int i = 0; i < 20000; i++) {
		__asm__ __volatile__ ("nop");
	}

	ap_ack = 0;
	lapic_send_startup_ipi(cpu_id, TRAMPOLINE_ADDR);
	while (!ap_ack)
		__barrier();
}

static int unit_init(void) {
	int self_id = lapic_id();

	/* Initialize trampoline for the APs */
	init_trampoline();

	/* Start all CPUs */
	for (int i = 0; i < NCPU; i++) {
		if (i != self_id)
			cpu_start(i);
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

