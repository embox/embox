/**
 * @file
 *
 * @date Mar 16, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_E2K_INCLUDE_ASM_SETJMP_H_
#define SRC_ARCH_E2K_INCLUDE_ASM_SETJMP_H_

#include <stdint.h>

#define _JBRNUM 7

#define THREAD_  (_JBRNUM + 0)
#define SIGNAL_  (_JBRNUM + 1)
#define SAVE_    (_JBRNUM + 2)
#define SIGMASK_ (_JBRNUM + 3)
#define KSTTOP   (_JBRNUM + 4)
#define _JBLEN   (_JBRNUM + 5)

#ifndef __ASSEMBLER__

struct _jump_regs {
	uint64_t cr0_hi;
	uint64_t cr1_lo;
	uint64_t cr1_hi;
	uint64_t pcsp_hi;
	uint64_t psp_hi;
	uint64_t usd_lo;
	uint64_t usd_hi;
};

typedef uint64_t __jmp_buf[_JBLEN];

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_E2K_INCLUDE_ASM_SETJMP_H_ */
