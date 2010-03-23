/**
 * @file
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 */

#ifndef MMU_CORE_H_
#define MMU_CORE_H_

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

#endif /* MMU_CORE_H_ */
