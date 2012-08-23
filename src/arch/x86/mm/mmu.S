#include <asm/flags.h>

	.global setup_gp_flag
setup_gp_flag:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	jmp	pipeline_flush1		/* Flush processor pipeline */
pipeline_flush1:
	movl	%cr0, %eax		/* Enable paging bit */
	orl	$(X86_CR0_PG), %eax
	movl	%eax, %cr0
	jmp	pipeline_flush		/* Flush processor pipeline */
pipeline_flush:
	movl	$cs_reset, %eax
	jmp	*%eax
cs_reset:
	movl	$(global_pdt), %edi	/* Unmap 0-4M */
	movl	$0, (%edi)
	movl	%cr3, %eax
	movl	%eax, %cr3
