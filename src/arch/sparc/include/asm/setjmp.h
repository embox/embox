/**
 * @file
 * @brief setjmp/longjmp for the sparc architecture
 *
 * @date 31.01.2010
 * @author Eldar Abusalimov
 */

#ifndef SPARC_SETJMP_H_
#define SPARC_SETJMP_H_

#include <types.h>

#ifndef __ASSEMBLER__

typedef struct {
	/* 0x0 */uint32_t fp;
	/* 0x4 */uint32_t i7;
	/* 0x8 */uint32_t sp;
	/* 0xc */uint32_t o7;
} __jmp_buf[1];

#else /* __ASSEMBLER__ */

/* Offsets for __jmp_buf */
#define JB_FP    0x0
#define JB_I7    0x4
#define JB_SP    0x8
#define JB_O7    0xc

#endif /* __ASSEMBLER__ */

#endif /* SPARC_SETJMP_H_ */
