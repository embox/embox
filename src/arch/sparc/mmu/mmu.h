/**
 * @file
 * @brief
 *
 * @date 20.07.2014
 * @author Alexander Kalmuk
 */

#ifndef MMU_H_
#define MMU_H_


#include <stdint.h>

#define MMU_LEVELS              3

#define __MMU_PGD_SHIFT         24
#define __MMU_PMD_SHIFT         18
#define __MMU_PTE_SHIFT         12

#define __MMU_SHIFT_0       __MMU_PGD_SHIFT
#define __MMU_SHIFT_1       __MMU_PMD_SHIFT
#define __MMU_SHIFT_2       __MMU_PTE_SHIFT

#ifndef __ASSEMBLER__
#include <inttypes.h>

typedef uint32_t __mmu_paddr_t;
typedef uint32_t __mmu_vaddr_t;

typedef uint32_t __mmu_ctx_t;

typedef uint32_t __mmu_reg_t;

#define __PRIxMMUREG PRIx32

#include <asm/asi.h>

static inline void mmu_set_mmureg(unsigned long addr_reg,
				unsigned long regval) {
	__asm__ __volatile__(
		"sta %0, [%1] %2\n\t"
		:
		: "r"(regval), "r"(addr_reg), "i"(ASI_M_MMUREGS)
		: "memory"
	);
}

static inline unsigned long mmu_get_mmureg(unsigned long addr_reg) {
	register int retval;
	__asm__ __volatile__(
		"lda [%1] %2, %0\n\t"
		: "=r" (retval)
		: "r" (addr_reg), "i" (ASI_M_MMUREGS)
	);
	return retval;
}

#endif

#define MMU_CTX_PMASK        0xfffffff0
#define MMU_PTD_PMASK        0xfffffff0
#define MMU_PTE_PMASK        0xffffff00

#define MMU_ET_PTD           0x1
#define MMU_ET_PTE           0x2

#define MMU_PAGE_WRITABLE    ((1UL << 0) << 2)
#define MMU_PAGE_EXECUTABLE  ((1UL << 1) << 2)
#define MMU_PAGE_SUPERVISOR  ((1UL << 2) << 2)

#define MMU_PAGE_CACHEABLE   (1UL << 7)


#endif /* MMU_H_ */
