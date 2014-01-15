/**
 * @file
 *
 * @date 26.11.10
 * @author Nikolay Korotky
 */

#include <asm/setjmp.h>

        .text

        .global setjmp
setjmp:
	movl    4(%esp), %ecx   /* fetch buffer */
	movl    %ebx, 0(%ecx)
	movl    %esi, 4(%ecx)
	movl    %edi, 8(%ecx)
	movl    %ebp, 12(%ecx)  /* save frame pointer of caller */
	popl    %edx
	movl    %esp, 16(%ecx)  /* save stack pointer of caller */
	movl    %edx, 20(%ecx)  /* save pc of caller */
	xorl    %eax, %eax
	jmp     *%edx

	.global longjmp
longjmp:
	movl    8(%esp), %eax    /* return(v) */
	movl    4(%esp), %ecx    /* fetch buffer */
	movl    0(%ecx), %ebx
	movl    4(%ecx), %esi
	movl    8(%ecx), %edi
	movl    12(%ecx), %ebp
	movl    16(%ecx), %esp
	orl     %eax, %eax
	jnz     0f
	incl    %eax
0:	jmp     *20(%ecx)         /* done, return.... */

	.global cpu_stack_move
cpu_stack_move:
	movl    4(%esp), %eax /* destanation stack */
	movl    8(%esp), %edx /* source stack */

	movl    %edx, %ecx
	subl    %esp, %ecx

	subl    %ecx, %eax
	subl    %ecx, %edx

	pushl   %ecx
	pushl   %edx
	pushl   %eax
	call    memcpy
	popl    %eax
	popl    %edx
	popl    %ecx

/* switch stack pointer */
	subl    %eax, %edx

	subl    %edx, %esp
	subl    %edx, %ebp

	ret

