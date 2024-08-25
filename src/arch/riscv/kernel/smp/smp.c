/**
 * @file
 * @brief
 *
 * @date 22.08.2024
 * @author Zeng Zixian
 */

#include <embox/unit.h>
#include <hal/cpu_idle.h>
#include <asm/interrupts.h>
#include <kernel/cpu/cpu.h>
#include <kernel/panic.h>
#include <kernel/spinlock.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/thread.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>

#include <drivers/irqctrl.h>
#include <module/embox/kernel/thread/core.h>
#include <drivers/interrupt/riscv_clint/riscv_clint.h>

#define THREAD_STACK_SIZE \
	OPTION_MODULE_GET(embox__kernel__thread__core, NUMBER, thread_stack_size)

EMBOX_UNIT_INIT(unit_init);

extern void *__ap_sp;
static char ap_stack[NCPU - 1][KERNEL_AP_STACK_SZ]
    __attribute__((aligned(KERNEL_AP_STACK_SZ)));
static int ap_ack;
static spinlock_t startup_lock = SPIN_STATIC_UNLOCKED;

static void *bs_idle_run(void *arg) {
	panic("%s runned\n", __func__);
}

extern void thread_set_current(struct thread *t);

void startup_ap(void) {
	struct thread *bs_idle;
	int self_id = cpu_get_id();

	__spin_lock(&startup_lock);

	/* enable external interrupt on each AP independently */
	irqctrl_init();

	/* enable external interrupt on each AP independently */
	clint_init();

	/* set up clock interrupt for each AP independently */
	((struct clock_source *)__riscv_timer_data)->event_device->set_periodic(__riscv_timer_data);

	bs_idle = thread_init_stack(__ap_sp - THREAD_STACK_SIZE, THREAD_STACK_SIZE,
	    SCHED_PRIORITY_MIN, bs_idle_run, NULL);
	cpu_init(self_id, bs_idle);
	task_thread_register(task_kernel_task(), bs_idle);
	thread_set_current(bs_idle);
	sched_set_current(&bs_idle->schedee);

	ap_ack = 1;

	__spin_unlock(&startup_lock);
	ipl_enable();

	while (1) {
		arch_cpu_idle();
	}
}


static inline void cpu_start(int cpu_id) {
	/* Setting up stack and boot */
	__ap_sp = (char *)&ap_stack[cpu_id] + KERNEL_AP_STACK_SZ;

	ap_ack = 0;

	clint_configure_msip(1,cpu_id);
	while (!ap_ack)
		__barrier();
}

static int unit_init(void) {
	int i, self_id;

	/* Start all CPUs */
	self_id = cpu_get_id();
	for (i = 0; i < NCPU; i++) {
		if (i != self_id)
			cpu_start(i);
	}

	return 0;
}

void smp_send_resched(int cpu_id) {
}

void resched(void) {
	extern void sched_post_switch(void);

	sched_post_switch();
}

