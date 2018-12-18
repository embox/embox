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

park:   # infinite wait for interrupts
	wfi
	j       park

trap_vector:
	# Save registers.
	addi    sp, sp, -CONTEXT_SIZE
	sxsp    ra, 0
	sxsp    a0, 1
	sxsp    a1, 2
	sxsp    a2, 3
	sxsp    a3, 4
	sxsp    a4, 5
	sxsp    a5, 6
	sxsp    a6, 7
	sxsp    a7, 8
	sxsp    t0, 9
	sxsp    t1, 10
	sxsp    t2, 11
	sxsp    t3, 12
	sxsp    t4, 13
	sxsp    t5, 14
	sxsp    t6, 15

	# Invoke the handler.
	mv      a0, sp
	csrr    a1, mcause
	csrr    a2, mepc
	jal     trap_handler

	# Restore registers.
	lxsp    ra, 0
	lxsp    a0, 1
	lxsp    a1, 2
	lxsp    a2, 3
	lxsp    a3, 4
	lxsp    a4, 5
	lxsp    a5, 6
	lxsp    a6, 7
	lxsp    a7, 8
	lxsp    t0, 9
	lxsp    t1, 10
	lxsp    t2, 11
	lxsp    t3, 12
	lxsp    t4, 13
	lxsp    t5, 14
	lxsp    t6, 15
	addi sp, sp, CONTEXT_SIZE

	# Return
	mret


	.bss
	.align 4
	.global stacks
stacks:
	.skip STACK_SIZE * MAX_HARTS
