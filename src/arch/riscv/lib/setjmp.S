#include <asm/asm.h>

.section .text

.global setjmp
.global longjmp

.align 2
setjmp:
	/* Preserve all callee-saved registers and the SP */
	PTR_S   s0, (SZREG * 0)(a0)
	PTR_S   s1, (SZREG * 1)(a0)
	PTR_S   s2, (SZREG * 2)(a0)
	PTR_S   s3, (SZREG * 3)(a0)
	PTR_S   s4, (SZREG * 4)(a0)
	PTR_S   s5, (SZREG * 5)(a0)
	PTR_S   s6, (SZREG * 6)(a0)
	PTR_S   s7, (SZREG * 7)(a0)
	PTR_S   s8, (SZREG * 8)(a0)
	PTR_S   s9, (SZREG * 9)(a0)
	PTR_S   s10, (SZREG * 10)(a0)
	PTR_S   s11, (SZREG * 11)(a0)
	PTR_S   ra, (SZREG * 12)(a0)
	PTR_S   sp, (SZREG * 13)(a0)

	li      a0, 0
	ret

.align 2
longjmp:
	PTR_L   s0, (SZREG * 0)(a0)
	PTR_L   s1, (SZREG * 1)(a0)
	PTR_L   s2, (SZREG * 2)(a0)
	PTR_L   s3, (SZREG * 3)(a0)
	PTR_L   s4, (SZREG * 4)(a0)
	PTR_L   s5, (SZREG * 5)(a0)
	PTR_L   s6, (SZREG * 6)(a0)
	PTR_L   s7, (SZREG * 7)(a0)
	PTR_L   s8, (SZREG * 8)(a0)
	PTR_L   s9, (SZREG * 9)(a0)
	PTR_L   s10, (SZREG * 10)(a0)
	PTR_L   s11, (SZREG * 11)(a0)
	PTR_L   ra, (SZREG * 12)(a0)
	PTR_L   sp, (SZREG * 13)(a0)

	/* Move the return value in place, but return 1 if passed 0. */
	beq     a1, zero, longjmp_1
	mv      a0, a1
	ret

longjmp_1:
	li      a0, 1
	ret
