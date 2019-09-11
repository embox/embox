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

#if 0
#define E2K_JMBBUFF_THREAD_  (_JBRNUM + 0)
#define E2K_JMBBUFF_SIGNAL_  (_JBRNUM + 1)
#define E2K_JMBBUFF_SAVE_    (_JBRNUM + 2)
#define E2K_JMBBUFF_SIGMASK_ (_JBRNUM + 3)
#define E2K_JMBBUFF_KSTTOP   (_JBRNUM + 4)
#endif

#define _JBLEN   (_JBRNUM )

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


#define E2K_JMBBUFF_CR0_HI    0x00
#define E2K_JMBBUFF_CR1_LO    0x08
#define E2K_JMBBUFF_CR1_HI    0x10
#define E2K_JMBBUFF_PCSP_HI   0x18
#define E2K_JMBBUFF_PSP_HI    0x20
#define E2K_JMBBUFF_USD_LO    0x28
#define E2K_JMBBUFF_USD_HI    0x30

#endif /* SRC_ARCH_E2K_INCLUDE_ASM_SETJMP_H_ */
