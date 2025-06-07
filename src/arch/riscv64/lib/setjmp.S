/**
 * @file
 * @brief RISC-V64 implementation of setjmp/longjmp.
 * @details This file provides context switching routines for RISC-V64 architecture.
 *          Implements standard setjmp/longjmp functionality with register saving.
 *
 * @date 06.06.2025
 * @author Wang Yulong <2302147681@qq.com>
 */

#include <asm/asm.h>

.text

.global setjmp
.global longjmp

/* 
 * int setjmp(jmp_buf env);
 * @brief Save current execution context
 * @param env Buffer to store execution context
 * @return 0 when called directly
 */
.align 4
setjmp:
    /* Save callee-saved registers */
    PTR_S   s0, (SETJMP_S0)(a0)
    PTR_S   s1, (SETJMP_S1)(a0)
    PTR_S   s2, (SETJMP_S2)(a0)
    PTR_S   s3, (SETJMP_S3)(a0)
    PTR_S   s4, (SETJMP_S4)(a0)
    PTR_S   s5, (SETJMP_S5)(a0)
    PTR_S   s6, (SETJMP_S6)(a0)
    PTR_S   s7, (SETJMP_S7)(a0)
    PTR_S   s8, (SETJMP_S8)(a0)
    PTR_S   s9, (SETJMP_S9)(a0)
    PTR_S   s10, (SETJMP_S10)(a0)
    PTR_S   s11, (SETJMP_S11)(a0)
    
    /* Save return address and stack pointer */
    PTR_S   ra, (SETJMP_RA)(a0)
    PTR_S   sp, (SETJMP_SP)(a0)

    /* Return 0 for direct call */
    li      a0, 0
    ret

/*
 * void longjmp(jmp_buf env, int val);
 * @brief Restore saved execution context
 * @param env Buffer containing saved context
 * @param val Value to return from setjmp
 * @note Never returns to the caller
 */
.align 4
longjmp:
    /* Restore callee-saved registers */
    PTR_L   s0, (SETJMP_S0)(a0)
    PTR_L   s1, (SETJMP_S1)(a0)
    PTR_L   s2, (SETJMP_S2)(a0)
    PTR_L   s3, (SETJMP_S3)(a0)
    PTR_L   s4, (SETJMP_S4)(a0)
    PTR_L   s5, (SETJMP_S5)(a0)
    PTR_L   s6, (SETJMP_S6)(a0)
    PTR_L   s7, (SETJMP_S7)(a0)
    PTR_L   s8, (SETJMP_S8)(a0)
    PTR_L   s9, (SETJMP_S9)(a0)
    PTR_L   s10, (SETJMP_S10)(a0)
    PTR_L   s11, (SETJMP_S11)(a0)
    
    /* Restore return address and stack pointer */
    PTR_L   ra, (SETJMP_RA)(a0)
    PTR_L   sp, (SETJMP_SP)(a0)

    /* Set return value (1 if val is 0) */
    beqz    a1, 1f
    mv      a0, a1
    ret

1:
    li      a0, 1
    ret