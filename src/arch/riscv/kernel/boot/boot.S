/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 06.07.23
 */

#include <asm/asm.h>
#include <asm/csr.h>
#include <hal/cpu.h>
#include <riscv/smode.h>

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

#ifdef __EMBOX_PLATFORM__mikron_mik32__
	la      t0, _start
#else
	la      t0, riscv_trap_handler
#endif
	csrw    CSR_TVEC, t0

#if RISCV_SMODE
	/**
	 * After SBI, a0 contains Hart ID of any
	 * possible Harts
	 */
	add     tp, a0, 0
#else
	/**
	 * Hart 0 is considered as BSP, other Harts
	 * go to secondary_hart_loop.
	 */
	csrr    t0, mhartid

#ifndef __EMBOX_PLATFORM__mikron_mik32__
	bnez    t0, secondary_hart_loop
#endif

#endif
	/* Set up stack pointer. */
	la      sp, _stack_top

	/* Zero the BSS section. */
	la      t0, _bss_vma
	la      t1, _bss_end
	bge     t0, t1, zero_bss_end
zero_bss:
	REG_S   zero, 0(t0)
	addi    t0, t0, REG_SIZE_X
	blt     t0, t1, zero_bss
zero_bss_end:

	/* Copy data section. */
	la      t0, _data_lma
	la      t1, _data_vma
	beq     t0, t1, copy_data_end
	la      t2, _data_len
	add     t2, t2, t0
copy_data:
	REG_L   t3, 0(t0)
	REG_S   t3, 0(t1)
	addi    t0, t0, REG_SIZE_X
	addi    t1, t1, REG_SIZE_X
	blt     t0, t2, copy_data
copy_data_end:

#if NCPU > 1
	call    cpudata_init
#endif /* NCPU > 1 */

#ifdef __riscv_f
	/* Enable FPU */
	lui     t0, (CSR_STATUS_FS_INIT >> 12)
	csrs    CSR_STATUS, t0
	csrw    fcsr, zero
#endif

	/* Start kernel. */
	call    kernel_start
.cfi_endproc

secondary_hart_loop:
#ifdef SMP
	csrw    CSR_STATUS, CSR_STATUS_IE
	/* Turn on soft interrput before WFI*/
	csrw    CSR_IE, CSR_IE_SIE

	la      t0, __ap_trampoline
	csrw    CSR_TVEC, t0
#endif /* SMP */
	wfi
	j       secondary_hart_loop

#ifdef __EMBOX_PLATFORM__mikron_mik32__
.section .mik32_trap_code, "ax"
//.org _start + 0xC0
.org 0x40
__mik32_trap_entry:
.cfi_startproc
	j       riscv_trap_handler
.cfi_endproc
#endif
