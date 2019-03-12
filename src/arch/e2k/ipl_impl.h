#ifndef E2K_IPL_IMPL_H_
#define E2K_IPL_IMPL_H_

#include <stdint.h>

typedef uint32_t __ipl_t;

/* UPSR register bits */
#define UPSR_IE   (1 << 5) /* Enable interrutps */
#define UPSR_NMIE (1 << 7) /* Enable non-maskable interrupts */

/* PSR register bits */
#define PSR_IE   (1 << 1) /* Enable interrutps */
#define PSR_NMIE (1 << 4) /* Enable non-maskable interrupts */
#define PSR_UIE  (1 << 5) /* Allow user to control interrupts */

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

#endif /* E2K_IPL_IMPL_H_ */
