/**
 * @file
 * @brief
 *
 * @date 26.11.13
 * @author Ilia Vaprol
 */

#ifndef ARM_HAL_MMU_H_
#define ARM_HAL_MMU_H_

/**
 * First-level descriptor format (VMSAv6, subpages disabled)
 */
#define L1D_TYPE_SD 0x00002 /* section descriptor */
#define L1D_B       0x00004 /* bufferable */
#define L1D_C       0x00008 /* cacheable */
#define L1D_XN      0x00010 /* not executable */
#define L1D_AP_FULL 0x00C00 /* full access permissions (APX must be 0) */
#define L1D_S       0x10000 /* shared */
#define L1D_NG      0x20000 /* not global */
#define L1D_BASE(n) (0x100000 + n) /* section base address */

#define L1D_TEX_OFFSET	5
#define L1D_TEX_MASK	(0x7 << L1D_TEX_OFFSET)
#define L1D_TEX_B	(1 << L1D_TEX_OFFSET)
#define L1D_TEX_C	(2 << L1D_TEX_OFFSET)
#define L1D_TEX_USE	(4 << L1D_TEX_OFFSET)

#endif /* ARM_HAL_MMU_H_ */
