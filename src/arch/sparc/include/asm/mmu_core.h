/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef MMU_CORE_H_
#define MMU_CORE_H_

#include <asm/asi.h>

/*FIXME move MMU_TABLE_SIZE to arch dependent config*/
#define MMU_TABLE_SIZE 0x400

/* mmu register access, ASI_LEON_MMUREGS */
#define LEON_CNR_CTRL           0x000
#define LEON_CNR_CTXP           0x100
#define LEON_CNR_CTX            0x200
#define LEON_CNR_F              0x300
#define LEON_CNR_FADDR          0x400

#define LEON_CNR_CTX_NCTX       256     /*number of MMU ctx */
#define LEON_CNR_CTRL_TLBDIS    0x80000000
#define LEON_MMUTLB_ENT_MAX     64

static inline void srmmu_set_mmureg(unsigned long regval) {
	__asm__ __volatile__("sta %0, [%%g0] %1\n\t" :
			: "r" (regval), "i" (ASI_M_MMUREGS)
			: "memory"
		);
}

static inline unsigned long srmmu_get_mmureg(unsigned long addr_reg) {
	register int retval;
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
			: "=r" (retval)
			: "r" (addr_reg), "i" (ASI_M_MMUREGS)
		);
	return retval;
}

#endif /* MMU_CORE_H_ */
