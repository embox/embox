/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 08.08.23
 */
#ifndef RISCV_ASM_CACHE_H_
#define RISCV_ASM_CACHE_H_

static inline void riscv_icache_flush(void) {
	__asm__ __volatile__("fence.i" ::: "memory");
}

#endif /* RISCV_ASM_CACHE_H_ */
