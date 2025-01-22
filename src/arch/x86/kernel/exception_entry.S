/**
 * @file
 *
 * @date 17.12.10
 * @author Nikolay Korotky
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text

	EXCEPTION(0x00, t_excep0)       /* Divide By Zero Exception */
	EXCEPTION(0x01, t_excep1)       /* Debug Exception */
	EXCEPTION(0x02, t_excep2)       /* Non Maskable Interrupt Exception */
	EXCEPTION(0x03, t_excep3)       /* Breakpoint Exception */
	EXCEPTION(0x04, t_excep4)       /* Into Detected Overflow Exception */
	EXCEPTION(0x05, t_excep5)       /* Out of Bounds Exception */
	EXCEPTION(0x06, t_excep6)       /* Invalid Opcode Exception */
	EXCEPTION(0x07, t_excep7)       /* No Coprocessor Exception */
	EXCEPTION_ERR(0x08, t_excep8)   /* Double Fault Exception */
	EXCEPTION(0x09, t_excep9)       /* Coprocessor Segment Overrun Exception */
	EXCEPTION_ERR(0x0a, t_excep10)  /* Bad TSS Exception */
	EXCEPTION_ERR(0x0b, t_excep11)  /* Segment Not Present Exception */
	EXCEPTION_ERR(0x0c, t_excep12)  /* Stack Fault Exception */
	EXCEPTION_ERR(0x0d, t_excep13)  /* General Protection Fault Exception */
	EXCEPTION_ERR(0x0e, t_excep14)  /* Page Fault Exception */
	EXCEPTION(0x0f, t_excep15)      /* Unknown Interrupt Exception */
	EXCEPTION(0x10, t_excep16)      /* Coprocessor Fault Exception */
	EXCEPTION_ERR(0x11, t_excep17)  /* Alignment Check Exception */
	EXCEPTION(0x12, t_excep18)      /* Machine Check Exception */
	EXCEPTION(0x13, t_excep19)      /* Reserved Exception */
	EXCEPTION(0x14, t_excep20)      /* Reserved Exception */
	EXCEPTION(0x15, t_excep21)      /* Reserved Exception */
	EXCEPTION(0x16, t_excep22)      /* Reserved Exception */
	EXCEPTION(0x17, t_excep23)      /* Reserved Exception */
	EXCEPTION(0x18, t_excep24)      /* Reserved Exception */
	EXCEPTION(0x19, t_excep25)      /* Reserved Exception */
	EXCEPTION(0x1a, t_excep26)      /* Reserved Exception */
	EXCEPTION(0x1b, t_excep27)      /* Reserved Exception */
	EXCEPTION(0x1c, t_excep28)      /* Reserved Exception */
	EXCEPTION(0x1d, t_excep29)      /* Reserved Exception */
	EXCEPTION(0x1e, t_excep30)      /* Reserved Exception */
	EXCEPTION(0x1f, t_excep31)      /* Reserved Exception */

	.section .traps.text

excep_stub:
	SAVE_ALL

	mov     %esp, %eax     /* Used for fastcall of handler */
	call    x86_excpt_handler

	RESTORE_ALL
