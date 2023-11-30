/**
 * @file
 *
 * @date 01.07.10
 * @author Anton Kozlov
 */

#ifndef ARM7_IPL_IMPL_H_
#define ARM7_IPL_IMPL_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

#include <hal/reg.h>

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
	ARCH_REG_CLEAR(CPSR, PSR_I | PSR_F);
}

static inline __ipl_t ipl_save(void) {
	__ipl_t ipl = ARCH_REG_LOAD(CPSR);
	ARCH_REG_STORE(CPSR, ipl | PSR_I | PSR_F);
	return ipl;
}

static inline void ipl_restore(__ipl_t ipl) {
	ARCH_REG_STORE(CPSR, ipl);
}

#endif /* __ASSEMBLER__ */

#endif /* ARM7_IPL_IMPL_H_ */
