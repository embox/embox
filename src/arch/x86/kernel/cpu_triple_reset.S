/**
 * @file
 *
 * @date Jan 25, 2013
 * @author: Anton Bondarev
 */
#include <asm/linkage.h>

	.section .text
	.align	4
/*
 * Reset cpu
 * Use triple fault
 */
C_ENTRY(cpu_triple_reset):
	cli
	movl	$null_idt, %eax		/* Reset by triple fault */
	lidt	(%eax)
	int	$3
	hlt

	.align  4
null_idt:
	.word	0
	.long	0
