/**
 * @file ipl_impl.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */

#ifndef AARCH64_IPL_IMPL_H_
#define AARCH64_IPL_IMPL_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

#include <hal/reg.h>

typedef uint64_t __ipl_t;

static inline void ipl_init(void) {
	ARCH_REG_STORE(DAIFClr, DAIF_I_imm | DAIF_F_imm);
}

static inline __ipl_t ipl_save(void) {
	__ipl_t r = ARCH_REG_LOAD(DAIF);
	ARCH_REG_STORE(DAIFSet, DAIF_I_imm | DAIF_F_imm);
	return r;
}

static inline void ipl_restore(__ipl_t ipl) {
	ARCH_REG_STORE(DAIF, ipl);
}

#endif /* __ASSEMBLER__ */

#endif /* AARCH64_IPL_IMPL_H_ */
