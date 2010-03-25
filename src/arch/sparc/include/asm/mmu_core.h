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

/**
 * Describes structure for MMU environment for SPARC architecture.
 */
typedef struct __mmu_env {
	uint32_t status;          /**< MMU enabled/disabled */

	uint32_t data_fault_cnt;  /**< Counter for data page faults */
	uint32_t inst_fault_cnt;  /**< Counter for instruction page faults */
	uint32_t fault_addr;      /**< Last fault address */
}__mmu_env_t;

#endif /* SPARC_MMU_CORE_H_ */
