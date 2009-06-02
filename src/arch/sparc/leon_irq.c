/*
 * \file leon_irq.c
 *
 * \date Jun 2, 2009
 * \author: anton
 */

#define PSR_PIL     0x00000f00         /* processor interrupt level  */

#define SMP_NOP2
#define SMP_NOP3

/**
 * disable interrupt for atpmic poperation
 * return old psr reg
 */
unsigned long __local_irq_save(void)
{
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
void local_irq_enable(void)
{
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
 * param psr which was returned __local_irq_save
 */
void local_irq_restore(unsigned long old_psr)
{
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
