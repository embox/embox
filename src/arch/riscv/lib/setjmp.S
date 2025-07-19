/**
 * @file
 * @brief
 *
 * @date 28.01.2020
 * @author Nastya Nizharadze
 */

#include <asm/asm.h>

#define __jmp_buf_ra_offset  (REG_SIZE_X * 0)
#define __jmp_buf_sp_offset  (REG_SIZE_X * 1)
#define __jmp_buf_s0_offset  (REG_SIZE_X * 2)
#define __jmp_buf_s1_offset  (REG_SIZE_X * 3)
#define __jmp_buf_s2_offset  (REG_SIZE_X * 4)
#define __jmp_buf_s3_offset  (REG_SIZE_X * 5)
#define __jmp_buf_s4_offset  (REG_SIZE_X * 6)
#define __jmp_buf_s5_offset  (REG_SIZE_X * 7)
#define __jmp_buf_s6_offset  (REG_SIZE_X * 8)
#define __jmp_buf_s7_offset  (REG_SIZE_X * 9)
#define __jmp_buf_s8_offset  (REG_SIZE_X * 10)
#define __jmp_buf_s9_offset  (REG_SIZE_X * 11)
#define __jmp_buf_s10_offset (REG_SIZE_X * 12)
#define __jmp_buf_s11_offset (REG_SIZE_X * 13)

.section .text
.global setjmp
.global longjmp

.align 2
setjmp:
	/* Store callee-saved registers */
	REG_S   ra, __jmp_buf_ra_offset(a0)
	REG_S   sp, __jmp_buf_sp_offset(a0)
	REG_S   s0, __jmp_buf_s0_offset(a0)
	REG_S   s1, __jmp_buf_s1_offset(a0)
	REG_S   s2, __jmp_buf_s2_offset(a0)
	REG_S   s3, __jmp_buf_s3_offset(a0)
	REG_S   s4, __jmp_buf_s4_offset(a0)
	REG_S   s5, __jmp_buf_s5_offset(a0)
	REG_S   s6, __jmp_buf_s6_offset(a0)
	REG_S   s7, __jmp_buf_s7_offset(a0)
	REG_S   s8, __jmp_buf_s8_offset(a0)
	REG_S   s9, __jmp_buf_s9_offset(a0)
	REG_S   s10, __jmp_buf_s10_offset(a0)
	REG_S   s11, __jmp_buf_s11_offset(a0)

	/* Return 0 */
	li      a0, 0
	ret

.align 2
longjmp:
	/* Load callee-saved registers */
	REG_L   ra, __jmp_buf_ra_offset(a0)
	REG_L   sp, __jmp_buf_sp_offset(a0)
	REG_L   s0, __jmp_buf_s0_offset(a0)
	REG_L   s1, __jmp_buf_s1_offset(a0)
	REG_L   s2, __jmp_buf_s2_offset(a0)
	REG_L   s3, __jmp_buf_s3_offset(a0)
	REG_L   s4, __jmp_buf_s4_offset(a0)
	REG_L   s5, __jmp_buf_s5_offset(a0)
	REG_L   s6, __jmp_buf_s6_offset(a0)
	REG_L   s7, __jmp_buf_s7_offset(a0)
	REG_L   s8, __jmp_buf_s8_offset(a0)
	REG_L   s9, __jmp_buf_s9_offset(a0)
	REG_L   s10, __jmp_buf_s10_offset(a0)
	REG_L   s11, __jmp_buf_s11_offset(a0)

	/* Return the second argument if it is non-zero, otherwise returns 1 */
	mv      a0, a1
	bnez    a1, 1f
	li      a0, 1
1:
	ret
