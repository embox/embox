/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.08.2013
 */

#include <asm/setjmp.h>
#include <module/embox/arch/msp430/abi.h>

#define RET_ADDR_REG MSP430_SARG4

.global setjmp
setjmp:
	pop	RET_ADDR_REG

	mov 	r1, 0(MSP430_SARG1)
	mov 	r2, 2(MSP430_SARG1)
	mov 	r4, 4(MSP430_SARG1)
	mov 	r5, 6(MSP430_SARG1)
	mov 	r6, 8(MSP430_SARG1)
	mov 	r7, 10(MSP430_SARG1)
	mov 	r8, 12(MSP430_SARG1)
	mov 	r9, 14(MSP430_SARG1)
	mov 	r10, 16(MSP430_SARG1)
	mov 	r11, 18(MSP430_SARG1)

	mov 	RET_ADDR_REG, 20(MSP430_SARG1)

	clr 	MSP430_SRET
	br	RET_ADDR_REG


.global longjmp
longjmp:
	mov 	@MSP430_SARG1+, r1
	mov 	@MSP430_SARG1+, r2
	mov 	@MSP430_SARG1+, r4
	mov 	@MSP430_SARG1+, r5
	mov 	@MSP430_SARG1+, r6
	mov 	@MSP430_SARG1+, r7
	mov 	@MSP430_SARG1+, r8
	mov 	@MSP430_SARG1+, r9
	mov 	@MSP430_SARG1+, r10
	mov 	@MSP430_SARG1+, r11

	mov 	@MSP430_SARG1, RET_ADDR_REG

	mov 	MSP430_SARG2, MSP430_SRET
	br	RET_ADDR_REG
