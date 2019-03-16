#ifndef E2K_IPL_IMPL_H_
#define E2K_IPL_IMPL_H_

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <e2k_api.h>

typedef uint32_t __ipl_t;

static inline void e2k_clear_upsr(uint32_t mask) {
	uint32_t upsr;
	asm volatile ("rrs %%upsr, %0" : "=r"(upsr) :);
	upsr &= ~mask;
	asm volatile ("rws %0, %%upsr" : : "ri"(upsr));
}

static inline void e2k_set_upsr(uint32_t mask) {
	uint32_t upsr;
	asm volatile ("rrs %%upsr, %0" : "=r"(upsr) :);
	upsr |= mask;
	asm volatile ("rws %0, %%upsr" : : "ri"(upsr));
}

static inline void ipl_init(void) {
	e2k_set_upsr(UPSR_IE | UPSR_NMIE);
}

static inline unsigned int ipl_save(void) {
	e2k_clear_upsr(UPSR_IE | UPSR_NMIE);
	return 0;
}

static inline void ipl_restore(unsigned int ipl) {
	e2k_set_upsr(UPSR_IE | UPSR_NMIE);
}

#endif /* __ASSEMBLER__ */

#endif /* E2K_IPL_IMPL_H_ */
