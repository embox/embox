/**
 * @file
 * @brief
 *
 * @date 30.08.2024
 * @author Zeng Zixian
 */

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <riscv/ipi.h>
#include <hal/cpu.h>
#include <drivers/interrupt/riscv_clint/riscv_clint.h>
static spinlock_t ipi_lock = SPIN_STATIC_UNLOCKED;
enum ipi_type ipi_message;

void smp_send_resched(int cpu_id) {
	__spin_lock(&ipi_lock);
	ipi_message = RESCHED;
	__spin_unlock(&ipi_lock);

	configure_soft_int(1,cpu_id);
}

void smp_send_none(int cpu_id) {
	__spin_lock(&ipi_lock);
	ipi_message = NONE;
	__spin_unlock(&ipi_lock);

	configure_soft_int(1,cpu_id);
}

void smp_ack_ipi(void) {
	int cpuid = cpu_get_id();
	configure_soft_int(0,cpuid);
}

enum ipi_type smp_get_ipi_message(void) {
	enum ipi_type message;

	__spin_lock(&ipi_lock);
	message = ipi_message;
	__spin_unlock(&ipi_lock);
	return message;
}
	

void resched(void) {
	extern void sched_post_switch_noyield(void);
	sched_post_switch_noyield();
}
