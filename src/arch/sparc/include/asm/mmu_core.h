/**
 * @file
 * @brief Defines specific features for MMU in SPARC architecture.
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef SPARC_MMU_CORE_H_
#define SPARC_MMU_CORE_H_

#include <asm/asi.h>

/* mmu register access, ASI_LEON_MMUREGS */
#define LEON_CNR_CTRL           0x000
#define LEON_CNR_CTXP           0x100
#define LEON_CNR_CTX            0x200
#define LEON_CNR_F              0x300
#define LEON_CNR_FADDR          0x400

#define LEON_CNR_CTX_NCTX       256     /*number of MMU ctx */
#define LEON_CNR_CTRL_TLBDIS    0x80000000

#define MMU_PAGE_SIZE         (1 << 12)
#define MMU_PTE_TABLE_SIZE    0x100 /* 64 entries, 4 bytes a piece  */
#define MMU_PMD_TABLE_SIZE    0x100 /* 64 entries, 4 bytes a piece  */
#define MMU_PGD_TABLE_SIZE    0x400 /* 256 entries, 4 bytes a piece */

/**
 * Describes structure for MMU environment for SPARC architecture.
 */
typedef struct __mmu_env {
	uint32_t status;          /**< MMU enabled/disabled */

	uint32_t data_fault_cnt;  /**< Counter for data page faults */
	uint32_t inst_fault_cnt;  /**< Counter for instruction page faults */
	uint32_t fault_addr;      /**< Last fault address */
}__mmu_env_t;

static inline void mmu_set_mmureg(unsigned long addr_reg,
				unsigned long regval) {
	__asm__ __volatile__("sta %0, [%1] %2\n\t"
		:
		: "r"(regval), "r"(addr_reg), "i"(ASI_M_MMUREGS)
		: "memory"
	);
}

static inline unsigned long mmu_get_mmureg(unsigned long addr_reg) {
	register int retval;
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
		: "=r" (retval)
		: "r" (addr_reg), "i" (ASI_M_MMUREGS)
	);
	return retval;
}

#endif /* SPARC_MMU_CORE_H_ */
