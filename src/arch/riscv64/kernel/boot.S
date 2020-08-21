.equ MAX_HARTS,		4
.equ STACK_SIZE,	1024

.section .text.init_start,"ax",@progbits
.global _start
.global die

_start:
.cfi_startproc
.cfi_undefined ra

	# setup default trap vector
	la		t0, trap_handler
	csrw	mtvec, t0

	# set up stack pointer based on hartid
	csrr	t0, mhartid

	# jump die if ID != 0
	bnez	t0, die

	# stack
	lla		sp, _stack_top

	# Zero the BSS segment
	la		t1, bss_target_start
	la		t2, bss_target_end

	bge		t1, t2, zero_bss_end

zero_bss:
	sw		x0, 0(t1)
	addi	t1, t1, 4
	blt		t1, t2, zero_bss
zero_bss_end:

	# jump kernel
	j		kernel_start
.cfi_endproc

die:
	j		die

