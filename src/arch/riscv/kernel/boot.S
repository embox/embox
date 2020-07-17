# code derived from https://github.com/michaeljclark/riscv-probe
.equ MAX_HARTS,    4
.equ STACK_SIZE,   1024
.equ STACK_SHIFT,  10
.equ REGBYTES, 4
.equ CONTEXT_SIZE, (16*REGBYTES) # SAVE_REGS * REGBYTES

# macros

.macro lx a, b
lw \a, \b
.endm

.macro sx a, b
sw \a, \b
.endm

.macro lxsp a, b
lw \a, ((\b)*REGBYTES)(sp)
.endm

.macro sxsp a, b
sw \a, ((\b)*REGBYTES)(sp)
.endm

.macro .ptr a
.4byte \a
.endm

.section .text.init_start,"ax",@progbits
.globl _start

_start:
.cfi_startproc

/* Inform the debugger that there is nowhere to backtrace past _enter. */
.cfi_undefined ra

/* The absolute first thing that must happen is configuring the global
 * pointer register, which must be done with relaxation disabled because
 * it's not valid to obtain the address of any symbol without GP
 * configured.  The C environment might go ahead and do this again, but
 * that's safe as it's a fixed register. */
.option push
.option norelax
	la gp, __global_pointer$
.option pop

	    li t6, 0x1800
	    csrw mstatus, t6

	# setup default trap vector
	la      t0, trap_handler
	csrw    mtvec, t0

	# set up stack pointer based on hartid
	csrr    t0, mhartid

	/* Stub. If there is more than one hart, we infinity loop all of them but
	 * one with ID = 0 */
	bne     t0, zero, die

	slli    t0, t0, STACK_SHIFT
	la      sp, stacks + STACK_SIZE
	add     sp, sp, t0

	/* Zero the BSS segment. */
	la t1, bss_target_start
	la t2, bss_target_end

	bge t1, t2, 2f

1:
	sw   x0, 0(t1)
	addi t1, t1, 4
	blt  t1, t2, 1b
2:

	j       kernel_start  # jump kernel
.cfi_endproc

die:
	j die

park:   # infinite wait for interrupts
	wfi
	j       park

	.bss
	.align 4
	.global stacks
stacks:
	.skip STACK_SIZE * MAX_HARTS
