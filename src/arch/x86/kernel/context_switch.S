/**
 * @file
 * @brief Context switching for x86.
 *
 * @date 26.02.11
 * @author Eldar Abusalimov
 */
#include <hal/context.h>
#include <module/embox/arch/usermode.h>

.text
.global __context_switch
__context_switch:
	movl    0x04(%esp), %ecx        /* Point ecx to previous registers */
	movl    (%esp), %eax            /* Get return address */
	movl    %eax, CTX_X86_EIP(%ecx) /* Save it as eip */
	movl    %ebx, CTX_X86_EBX(%ecx) /* Save ebx */
	movl    %edi, CTX_X86_EDI(%ecx) /* Save edi */
	movl    %esi, CTX_X86_ESI(%ecx) /* Save esi */
	movl    %ebp, CTX_X86_EBP(%ecx) /* Save ebp */
	add     $4, %esp                /* Move esp in state corresponding to eip */
	movl    %esp, CTX_X86_ESP(%ecx) /* Save esp */
	pushf                           /* Push flags */
	pop     CTX_X86_EFLAGS(%ecx)    /* ...and save them */

	movl    0x04(%esp), %ecx        /* Point ecx to next registers */
	movl    CTX_X86_EBX(%ecx), %ebx /* Restore ebx */
	movl    CTX_X86_EDI(%ecx), %edi /* Restore edi */
	movl    CTX_X86_ESI(%ecx), %esi /* Restore esi */
	movl    CTX_X86_EBP(%ecx), %ebp /* Restore ebp */
	movl    CTX_X86_ESP(%ecx), %esp /* Restore esp */
	push    CTX_X86_EFLAGS(%ecx)    /* Push saved flags */
	popf                            /* Restore flags */
	movl    CTX_X86_EIP(%ecx), %eax /* Get eip */
	push    %eax                    /* Restore it as return address */

#ifndef NOUSERMODE
	call    tss_set_kernel_stack
#endif /* NOUSERMODE */

	ret
