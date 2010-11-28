/**
 * @file
 * @brief Microblaze definition objects for setjmp
 *
 * @date 11.03.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_SETJMP_H_
#define MICROBLAZE_SETJMP_H_

#include <types.h>

#ifndef __ASSEMBLER__

typedef struct {
	/* 0x0 */uint32_t sp; /* Stack pointer.  */
	/* 0x4 */uint32_t lp; /* Link pointer.  */
	/* 0x8 */uint32_t regs[13]; /* Callee-saved registers r18-r30.  */
} __jmp_buf[1];

#endif /* __ASSEMBLER__ */

#define JB_SP        0x00
#define JB_LP        0x04
#define JB_REGS      0x08

#define JB_SIZE     (4 * 15)

#endif /* MICROBLAZE_SETJMP_H_ */
