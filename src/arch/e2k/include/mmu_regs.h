/**
 * @file
 *
 * @date Apr 16, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_E2K_INCLUDE_MMU_REGS_H_
#define SRC_ARCH_E2K_INCLUDE_MMU_REGS_H_

#include <e2k_api.h>

#define MMU_REG_CR          0x00  /* Control register */
#define MMU_REG_CONT        0x10  /* Context register */
#define MMU_REG_CR3_RG      0x20  /* CR3 register for INTEL only */
#define MMU_REG_ELB_PTB     0x30  /* ELBRUS page table virtual base */
#define MMU_REG_ROOT_PTB    0x40  /* Root Page Table Base register */
#define MMU_REG_TRAP_POINT  0x50  /* Trap Pointer register */
#define MMU_REG_TRAP_COUNT  0x60  /* Trap Counter register */


#define _MMU_CR_TLB_EN		0x0000000000000001	/* translation enable */
#define _MMU_CR_CD_MASK		0x0000000000000006	/* cache disable bits */
#define _MMU_CR_SET1		0x0000000000000008	/* set #1 enable for */
							/* 4 MB pages */
#define _MMU_CR_SET2		0x0000000000000010	/* set #2 enable for */
							/* 4 MB pages */
#define _MMU_CR_SET3		0x0000000000000020	/* set #3 enable for */
							/* 4 MB pages */
#define _MMU_CR_CR0_PG		0x0000000000000040	/* paging enable for */
							/* second space INTEL */
#define _MMU_CR_CR4_PSE		0x0000000000000080	/* page size 4Mb */
							/* enable for second */
							/* space INTEL */
#define _MMU_CR_CR0_CD		0x0000000000000100	/* cache disable for */
							/* secondary space */
							/* INTEL */
#define _MMU_CR_TLU2_EN		0x0000000000000200	/* TLU enable for */
							/* secondary space */
							/* INTEL */
#define _MMU_CR_LD_MPT		0x0000000000000400	/* memory protection */
							/* table enable for */
							/* LD from secondary */
							/* space INTEL */
#define _MMU_CR_IPD_MASK	0x0000000000000800	/* Instruction */
							/* Prefetch Depth */
#define _MMU_CR_UPT_EN		0x0000000000001000	/* enable UPT */

/*
 * Write MMU register
 */
#define WRITE_MMU_REG(addr_val, reg_val) \
		E2K_WRITE_MAS_D((addr_val), (reg_val), MAS_MMU_REG)

/*
 * Read MMU register
 */
#define READ_MMU_REG(addr_val)	\
		E2K_READ_MAS_D((addr_val), MAS_MMU_REG)

/*
 * Read MMU Control register
 */
#define READ_MMU_CR() \
		READ_MMU_REG(MMU_REG_CR)

/*
 * Write MMU Control register
 */
#define WRITE_MMU_CR(mmu_cr) \
		WRITE_MMU_REG(MMU_REG_CR, mmu_cr)

/*
 * Read MMU Control register
 */
#define READ_MMU_CONT() \
		READ_MMU_REG(MMU_REG_CONT)

/*
 * Write MMU Context register
 */
#define WRITE_MMU_CONT(mmu_cont) \
		WRITE_MMU_REG(MMU_REG_CONT, mmu_cont)

/*
 * Read MMU Control Register of secondary space table
 */
#define READ_MMU_CR3_RG() \
		READ_MMU_REG(MMU_REG_CR3_RG)

/*
 * Write MMU Control Register of secondary space table
 */
#define WRITE_MMU_CR3_RG(mmu_page_dir) \
		WRITE_MMU_REG(MMU_REG_CR3_RG,mmu_page_dir)

/*
 * Read MMU ELBRUS page table base register
 */
#define READ_MMU_ELB_PTB() \
		READ_MMU_REG(MMU_REG_ELB_PTB)

/*
 * Write MMU ELBRUS page table base register
 */
#define WRITE_MMU_ELB_PTB(mmu_elb_ptb) \
		WRITE_MMU_REG(MMU_REG_ELB_PTB, mmu_elb_ptb)

/*
 * Read MMU root page table base register
 */
#define READ_MMU_ROOT_PTB() \
		READ_MMU_REG(MMU_REG_ROOT_PTB)

/*
 * Write MMU root page table base register
 */
#define WRITE_MMU_ROOT_PTB(mmu_root_ptb) \
		WRITE_MMU_REG(MMU_REG_ROOT_PTB, mmu_root_ptb)

/*
 * CACHE(s) flush operations
 */

/* CACHE(s) flush operations address */

#define _FLUSH_INVALIDATE_CACHE_ALL_OP 0x0000000000000000
#define _FLUSH_WRITE_BACK_CACHE_ALL_OP 0x0000000000000001

#define _flush_op_invalidate_cache_all (long)_FLUSH_INVALIDATE_CACHE_ALL_OP
#define _flush_op_write_back_cache_all (long)_FLUSH_WRITE_BACK_CACHE_ALL_OP

/*
 * ICACHE/TLB flush operations
 */

/* ICACHE/TLB flush operations address */

#define _FLUSH_ICACHE_ALL_OP    0x0000000000000000
#define _FLUSH_TLB_ALL_OP       0x0000000000000001

#define _flush_op_icache_all    (long)_FLUSH_ICACHE_ALL_OP
#define _flush_op_tlb_all       (long)_FLUSH_TLB_ALL_OP

#endif /* SRC_ARCH_E2K_INCLUDE_MMU_REGS_H_ */
