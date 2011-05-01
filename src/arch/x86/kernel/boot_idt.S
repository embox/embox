/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#include <asm/multiboot.h>

#define EXCEPTION(n, name)       \
	.globl name             ;\
name:                           ;\
	cli                     ;\
	pushl	$(0)		;\
	pushl	$(n)		;\
	jmp	excep_stub

#define IRQ_ENTRY(n)      \
	.globl irq##n    ;\
irq##n:                  ;\
	cli              ;\
	pushl   $(0)     ;\
	pushl   $(32 + n);\
	jmp     irq_stub

EXCEPTION(0x00, t_excep0)    /* Divide By Zero Exception */
EXCEPTION(0x01, t_excep1)    /* Debug Exception */
EXCEPTION(0x02, t_excep2)    /* Non Maskable Interrupt Exception */
EXCEPTION(0x03, t_excep3)    /* Breakpoint Exception */
EXCEPTION(0x04, t_excep4)    /* Into Detected Overflow Exception */
EXCEPTION(0x05, t_excep5)    /* Out of Bounds Exception */
EXCEPTION(0x06, t_excep6)    /* Invalid Opcode Exception */
EXCEPTION(0x07, t_excep7)    /* No Coprocessor Exception */
EXCEPTION(0x08, t_excep8)    /* Double Fault Exception */
EXCEPTION(0x09, t_excep9)    /* Coprocessor Segment Overrun Exception */
EXCEPTION(0x0a, t_excep10)   /* Bad TSS Exception */
EXCEPTION(0x0b, t_excep11)   /* Segment Not Present Exception */
EXCEPTION(0x0c, t_excep12)   /* Stack Fault Exception */
EXCEPTION(0x0d, t_excep13)   /* General Protection Fault Exception */
EXCEPTION(0x0e, t_excep14)   /* Page Fault Exception */
EXCEPTION(0x0f, t_excep15)   /* Unknown Interrupt Exception */
EXCEPTION(0x10, t_excep16)   /* Coprocessor Fault Exception */
EXCEPTION(0x11, t_excep17)   /* Alignment Check Exception */
EXCEPTION(0x12, t_excep18)   /* Machine Check Exception */
EXCEPTION(0x13, t_excep19)   /* Reserved Exception */
EXCEPTION(0x14, t_excep20)   /* Reserved Exception */
EXCEPTION(0x15, t_excep21)   /* Reserved Exception */
EXCEPTION(0x16, t_excep22)   /* Reserved Exception */
EXCEPTION(0x17, t_excep23)   /* Reserved Exception */
EXCEPTION(0x18, t_excep24)   /* Reserved Exception */
EXCEPTION(0x19, t_excep25)   /* Reserved Exception */
EXCEPTION(0x1a, t_excep26)   /* Reserved Exception */
EXCEPTION(0x1b, t_excep27)   /* Reserved Exception */
EXCEPTION(0x1c, t_excep28)   /* Reserved Exception */
EXCEPTION(0x1d, t_excep29)   /* Reserved Exception */
EXCEPTION(0x1e, t_excep30)   /* Reserved Exception */
EXCEPTION(0x1f, t_excep31)   /* Reserved Exception */

IRQ_ENTRY(0) /* system timer (cannot be changed) */
IRQ_ENTRY(1) /* keyboard controller (cannot be changed) */
IRQ_ENTRY(2) /* cascaded signals from IRQs 8–15 */
IRQ_ENTRY(3) /* serial port controller for COM2(shared with COM4, if present) */
IRQ_ENTRY(4) /* serial port controller for COM1(shared with COM3, if present) */
IRQ_ENTRY(5) /* LPT port 2  or  sound card */
IRQ_ENTRY(6) /* floppy disk controller */
IRQ_ENTRY(7) /* LPT port 1 or sound card(8-bit Sound Blaster and compatibles) */

IRQ_ENTRY(8)  /* real-time clock */
IRQ_ENTRY(9)  /* open interrupt/available or SCSI host adapter */
IRQ_ENTRY(10) /* open interrupt / available  or  SCSI  or  NIC */
IRQ_ENTRY(11) /* open interrupt / available  or  SCSI  or  NIC */
IRQ_ENTRY(12) /* mouse on PS/2 connector */
IRQ_ENTRY(13) /* math co-processor/integrated FPU/inter-processor interrupt(use depends on OS) */
IRQ_ENTRY(14) /* primary ATA channel */
IRQ_ENTRY(15) /* secondary ATA channel */

excep_stub:
	pusha
	pushl	%ds
	pushl	%es
	pushl	%fs
	pushl	%gs
	movl	%ss, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%esp, %eax

	call    EXT_C(exception_handler)

	pop     %gs
	pop     %fs
	pop     %es
	pop     %ds
	popa
	add     $8, %esp
	sti
	iret

irq_stub:
	/* At this point on the stack there is:
	 *  0(%esp) vector
	 *  4(%esp) error code
	 *  8(%esp) eip
	 * 12(%esp) cs
	 * 16(%esp) eflags
	 */
	pusha /*pushes edi,esi,ebp,esp,ebx,edx,ecx,eax*/

#if 1
	pushl   %ds
	pushl   %es
	pushl   %fs
	pushl   %gs

	movl    %ss, %eax
	movl    %eax, %ds
	movl    %eax, %es
	movl    %esp, %eax
#else
	movw %ds, %ax /* Lower 16-bits of eax = ds. */
	pushl %eax    /* Save the data segment descriptor. */

	movw  $0x10, %ax /* Load the kernel data segment descriptor */
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs

	pushl %esp /* Push a pointer to the current top of stack -
	              this becomes the registers_t* parameter. */
#endif
	call    EXT_C(irq_handler)
#if 0
	add $4,%esp /* Remove the registers_t* parameter. */

	popl %ebx   /* Reload the original data segment descriptor */
#endif
	pop     %gs
	pop     %fs
	pop     %es
	pop     %ds

	popa
	add     $8, %esp
	sti
	iret
