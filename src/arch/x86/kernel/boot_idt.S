/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#define	GATE_ENTRY(n,entry,type) \
	.text	1		;\
	.long	entry		;\
	.word	n		;\
	.word	type		;\
	.text

#define	EXCEPTION(n,name)	\
	GATE_ENTRY(n,1f,0x0e)	;\
1:	pushl	$(0)		;\
	pushl	$(n)		;\
	jmp	2f

#define	EXCEP_USR(n,name)	\
	GATE_ENTRY(n,1f,0x6e)	;\
1:	pushl	$(0)		;\
	pushl	$(n)		;\
	jmp	2f

#define	EXCEP_ERR(n,name)	\
	GATE_ENTRY(n,1f,0x0e)	;\
1:	pushl	$(n)		;\
	jmp	2f

	.text   1
	.globl  boot_idt_inittab
boot_idt_inittab:
	.text

EXCEPTION(0x00, t_zero_div)
EXCEPTION(0x01, t_debug)
EXCEPTION(0x02, t_nmi)
EXCEP_USR(0x03, t_int3)
EXCEP_USR(0x04, t_into)
EXCEP_USR(0x05, t_bounds)
EXCEPTION(0x06, t_invop)
EXCEPTION(0x07, t_nofpu)
GATE_ENTRY(0x08, 0x20, 0x05)
EXCEPTION(0x09, a_fpu_over)
EXCEP_ERR(0x0a, a_inv_tss)
EXCEP_ERR(0x0b, t_segnp)
EXCEP_ERR(0x0c, t_stack_fault)
EXCEP_ERR(0x0d, t_gen_prot)
EXCEP_ERR(0x0e, t_page_fault)
EXCEPTION(0x0f, t_trap_0f)
EXCEPTION(0x10, t_fpu_err)
EXCEPTION(0x11, t_trap_11)
EXCEPTION(0x12, t_trap_12)
EXCEPTION(0x13, t_trap_13)

	.text   1
	.long   0
	.text

2:	pusha
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	movl	%ss, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%esp, %eax
	call	bad_trap_handler

