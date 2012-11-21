/**
 * @file
 * @brief
 *
 * @date 16.11.2012
 * @author Anton Bulychev
 */

#ifndef SPARC_MMU_CONSTS_H_
#define SPARC_MMU_CONSTS_H_

#define LEON_CNR_CTRL        0x00000000 /** Control Register */
#define LEON_CNR_CTXP        0x00000100 /** Context Table Pointer Register */
#define LEON_CNR_CTX         0x00000200 /** Context Register */
#define LEON_CNR_F           0x00000300 /** Fault Status Register */
#define LEON_CNR_FADDR       0x00000400 /** Fault Address Register */

#define MMU_CTX_PMASK        0xfffffff0
#define MMU_PTD_PMASK        0xfffffff0
#define MMU_PTE_PMASK        0xffffff00

#define MMU_ET_PTD           0x1
#define MMU_ET_PTE           0x2

#define MMU_ET_PRESENT       0x3

#define MMU_PAGE_WRITABLE    ((1UL << 0) << 2)
#define MMU_PAGE_SOMEFLAG    ((1UL << 1) << 2)
#define MMU_PAGE_SUPERVISOR  ((1UL << 2) << 2)

#define MMU_PAGE_CACHEABLE   (1UL << 7)

#endif

