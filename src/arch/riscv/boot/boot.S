/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.07.23
 */
#include <asm/asm.h>
#include <asm/regs.h>
#include <hal/cpu.h>

.section .init, "ax"

.globl _start

_start:
.cfi_startproc

/* Inform the debugger that there is nowhere to backtrace past _start. */
.cfi_undefined ra

.option push
.option norelax
	/**
	 * The absolute first thing that must happen is configuring the global
	 * pointer register, which must be done with relaxation disabled because
	 * it's not valid to obtain the address of any symbol without GP
	 * configured. The C environment might go ahead and do this again, but
	 * that's safe as it's a fixed register.
	 */
	la      gp, __global_pointer$
.option pop

	/**
	 * Set up mtvec (Machine Trap Vector). Whenever an exception or
	 * interrupt occurs, the CPU will "call" the function given by this
	 * register.
	 */
	la      t0, riscv_trap_handler
#if OPTION_GET(BOOLEAN,mik32_fixup)	
	la      t0, _start
#endif
	csrw    TRAP_VECTOR_REG, t0

#if OPTION_GET(BOOLEAN,smp_support)
	/**
	 * Continue on hart lottery winner, others branch to
	 * secondary_hart_loop.
	 */
#if SMODE
	add     t0, a0, 0
#else
	csrr    t0, mhartid
#endif
	bnez    t0, secondary_hart_loop
#endif /* OPTION_GET(BOOLEAN,smp_support) */

	/* Set up stack pointer. */
	la      sp, _stack_top

	/* Zero the BSS section. */
	la      t0, _bss_vma
	la      t1, _bss_end
	bge     t0, t1, zero_bss_end
zero_bss:
	PTR_S   zero, 0(t0)
	addi    t0, t0, SZREG
	blt     t0, t1, zero_bss
zero_bss_end:

	/* Copy data section. */
	la      t0, _data_lma
	la      t1, _data_vma
	beq     t0, t1, copy_data_end
	la      t2, _data_len
	add     t2, t2, t0
copy_data:
	PTR_L   t3, 0(t0)
	PTR_S   t3, 0(t1)
	addi    t0, t0, SZREG
	addi    t1, t1, SZREG
	blt     t0, t2, copy_data
copy_data_end:

#if NCPU > 1
	call    cpudata_init
#endif /* NCPU > 1 */

	/* Start kernel. */
	call    kernel_start
.cfi_endproc

secondary_hart_loop:
#ifdef SMP
	csrw    STATUS_REG, STATUS(IE)      
	/* Turn on soft interrput before WFI*/
	csrw    INTERRUPT_REG, IE(SIE) 

	la      t0, __ap_trampoline
	csrw    TRAP_VECTOR_REG, t0
#endif /* SMP */
	wfi
	j       secondary_hart_loop


#if OPTION_GET(BOOLEAN,mik32_fixup)
.section .mik32_fixup, "ax"

.globl _mik32_fixup_interrupt
//.org _start + 0xC0
.org 0x40
_mik32_fixup_interrupt:
.cfi_startproc
	j riscv_trap_handler
.cfi_endproc
#endif

