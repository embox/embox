#include <asm/asm.h>

.section .text

.global setjmp
.global longjmp

.align 2
setjmp:
	/* Preserve all callee-saved registers and the SP */
	REG_S   s0, (REG_SIZE_X * 0)(a0)
	REG_S   s1, (REG_SIZE_X * 1)(a0)
	REG_S   s2, (REG_SIZE_X * 2)(a0)
	REG_S   s3, (REG_SIZE_X * 3)(a0)
	REG_S   s4, (REG_SIZE_X * 4)(a0)
	REG_S   s5, (REG_SIZE_X * 5)(a0)
	REG_S   s6, (REG_SIZE_X * 6)(a0)
	REG_S   s7, (REG_SIZE_X * 7)(a0)
	REG_S   s8, (REG_SIZE_X * 8)(a0)
	REG_S   s9, (REG_SIZE_X * 9)(a0)
	REG_S   s10, (REG_SIZE_X * 10)(a0)
	REG_S   s11, (REG_SIZE_X * 11)(a0)
	REG_S   ra, (REG_SIZE_X * 12)(a0)
	REG_S   sp, (REG_SIZE_X * 13)(a0)

	li      a0, 0
	ret

.align 2
longjmp:
	REG_L   s0, (REG_SIZE_X * 0)(a0)
	REG_L   s1, (REG_SIZE_X * 1)(a0)
	REG_L   s2, (REG_SIZE_X * 2)(a0)
	REG_L   s3, (REG_SIZE_X * 3)(a0)
	REG_L   s4, (REG_SIZE_X * 4)(a0)
	REG_L   s5, (REG_SIZE_X * 5)(a0)
	REG_L   s6, (REG_SIZE_X * 6)(a0)
	REG_L   s7, (REG_SIZE_X * 7)(a0)
	REG_L   s8, (REG_SIZE_X * 8)(a0)
	REG_L   s9, (REG_SIZE_X * 9)(a0)
	REG_L   s10, (REG_SIZE_X * 10)(a0)
	REG_L   s11, (REG_SIZE_X * 11)(a0)
	REG_L   ra, (REG_SIZE_X * 12)(a0)
	REG_L   sp, (REG_SIZE_X * 13)(a0)

	/* Move the return value in place, but return 1 if passed 0. */
	beq     a1, zero, longjmp_1
	mv      a0, a1
	ret

longjmp_1:
	li      a0, 1
	ret
