/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.07.23
 */
#include <asm/asm.h>
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
	csrw    mtvec, t0

	/**
	 * Continue on hart lottery winner, others branch to
	 * secondary_hart_loop.
	 */
	csrr    t0, mhartid
	bnez    t0, secondary_hart_loop

	/* Set up stack pointer. */
	la      sp, _stack_top

	/* Zero the BSS segment. */
	la      t1, _bss_vma
	la      t2, _bss_end
	bge     t1, t2, zero_bss_end
zero_bss:
	sw      x0, 0(t1)
	addi    t1, t1, 4
	blt     t1, t2, zero_bss
zero_bss_end:

	/* Start kernel. */
	call    kernel_start
.cfi_endproc

secondary_hart_loop:
	wfi
#ifdef SMP	
	/* TODO: SMP support */
#endif
	j       secondary_hart_loop
