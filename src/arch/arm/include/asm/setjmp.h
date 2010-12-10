/**
 * @file
 * @brief setjmp/longjmp for the arm architecture
 *
 * @date 25.11.2010
 * @author Anton Kozlov
 */

#ifndef ARM_SETJMP_H_
#define ARM_SETJMP_H_

#include <types.h>

typedef struct {
	/* 0x0 */uint32_t cpsr;
	/* 0x4 */uint32_t r12;
	/* 0x8 */uint32_t lr;
	/* 0xc */uint32_t pc;
} __jmp_buf[1];

#endif /*ARM_SETJMP_H_*/
