/**
 * @file sys.c
 *
 * @date 09.06.09
 * @author Eldar Abusalimov
 * @author Alexey Fomin
 */

#include <kernel/sys.h>

void context_save(CPU_CONTEXT * pcontext) {
	__asm__ __volatile__(
			"mov %0, %%o0\n\t"
			"ta 5\n\t"
			"nop\n\t"::
			"r" (pcontext):
			"%o0");
}

void context_restore(CPU_CONTEXT * pcontext) {
	__asm__ __volatile__(
			"mov %0, %%o0\n\t"
			"ta 6\n\t"
			"nop\n\t"::
			"r" (pcontext):
			"%o0");
}

static CPU_CONTEXT context;
volatile static bool started = false;

int sys_exec_start(EXEC_FUNC f, int argc, char **argv) {
	if (started) {
		return -3;
	}

	int ret = -2;

	context_save(&context);

	if (!started) {
		started = true;
		ret = f(argc, argv);
	}
	started = false;

	return ret;
}

void sys_exec_stop() {
	if (!started) {
		return;
	}
	context_restore(&context);
}

void sys_halt() {
	//todo must be trap
}

bool sys_exec_is_started() {
	return started;
}

#define PSR_PIL     0x00000f00         /* processor interrupt level  */

#define SMP_NOP2
#define SMP_NOP3

/**
 * disable interrupt for atomic operation
 * return old psr reg
 */
unsigned long local_irq_save(void) {
	unsigned long retval;
	unsigned long tmp;

	__asm__ __volatile__(
		"rd	%%psr, %0\n\t"
		SMP_NOP3	/* Sun4m + Cypress + SMP bug */
		"or	%0, %2, %1\n\t"
		"wr	%1, 0, %%psr\n\t"
		"nop; nop; nop\n"
		: "=&r" (retval), "=r" (tmp)
		: "i" (PSR_PIL)
		: "memory");

	return retval;
}

/**
 * set PSR_PIL in 0xF
 */
void local_irq_enable(void) {
	unsigned long tmp;

	__asm__ __volatile__(
		"rd	%%psr, %0\n\t"
		SMP_NOP3	/* Sun4m + Cypress + SMP bug */
		"andn	%0, %1, %0\n\t"
		"wr	%0, 0, %%psr\n\t"
		"nop; nop; nop\n"
		: "=&r" (tmp)
		: "i" (PSR_PIL)
		: "memory");
}

/**
 * restore PSR_PIL after __local_irq_save
 * @param psr which was returned __local_irq_save
 */
void local_irq_restore(unsigned long old_psr) {
	unsigned long tmp;

	__asm__ __volatile__(
		"rd	%%psr, %0\n\t"
		"and	%2, %1, %2\n\t"
		SMP_NOP2	/* Sun4m + Cypress + SMP bug */
		"andn	%0, %1, %0\n\t"
		"wr	%0, %2, %%psr\n\t"
		"nop; nop; nop\n"
		: "=&r" (tmp)
		: "i" (PSR_PIL), "r" (old_psr)
		: "memory");
}
