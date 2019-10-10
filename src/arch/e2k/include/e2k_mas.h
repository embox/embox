#ifndef __E2K_MAS_H_
#define __E2K_MAS_H_

/*
 * Memory Address Specifier.
 *
 *       +-----------------------+-------+
 *       |           opc         |       |
 *       +-------+-------+-------+  mod  +
 *       |   dc  |   na  |   be  |       |
 *       +-------+-------+-------+-------+
 *       6-----5    4       3    2-----0
 *
 *        be      - big endian flag
 *        na      - non-aligned access flag
 *        dc      - DCACHEs disable flag
 *        opc     - special MMU or AAU operation opcode
 *        mod     - operation modifier
 *
 */

/* MAS masks */

#define MAS_MOD_MASK                     0x07
#define MAS_OPC_MASK                     0x78
#define MAS_ENDIAN_MASK                  0x08
#define MAS_NONALIGNED_MASK              0x10
#define MAS_DCACHE_MASK                  0x60

/* MAS bits */

#define MAS_MOD_BITS                     3
#define MAS_OPC_BITS                     4
#define MAS_ENDIAN_BITS                  1
#define MAS_NONALIGNED_BITS              1
#define MAS_DCACHE_BITS                  2

/* MAS shifts */

#define MAS_MOD_SHIFT                    0x00
#define MAS_OPC_SHIFT                    0x03
#define MAS_ENDIAN_SHIFT                 0x03
#define MAS_NONALIGNED_SHIFT             0x04
#define MAS_DCACHE_SHIFT                 0x05

#define MAS_MOD_DEFAULT                  0x0UL
#define MAS_ENDIAN_DEFAULT               0x0UL
#define MAS_NONALIGNED_DEFAULT           0x0UL
#define MAS_DCACHE_DEFAULT               0x0UL

/* LOAD (non-speculative) MAS modes for channels 0 and 3 */

#define _MAS_MODE_LOAD_OPERATION         0UL /* an operation */
#define _MAS_MODE_LOAD_PA                1UL /* reading by physical address */
/* conditional operation depending on the "check" lock state */
#define _MAS_MODE_LOAD_OP_CHECK          2UL
/* an operation with memory location depending on the "check" lock state and
 *  memory location unlocking */
#define _MAS_MODE_LOAD_OP_UNLOCK         3UL
#define _MAS_MODE_LOAD_OP_SPEC           3UL /* speculative load */
#define _MAS_MODE_FILL_OP                4UL /* fill operation */
/* semi-speculative operation with memory lock check*/
#define _MAS_MODE_LOAD_OP_LOCK_CHECK     4UL
/* an operation with locking of the memory location by the
 * "trap on store"lock*/
#define _MAS_MODE_LOAD_OP_TRAP_ON_STORE  5UL
 /* an operation with locking of the memory location by the
  * "trap on load/store"*/
#define _MAS_MODE_LOAD_OP_TRAP_ON_LD     6UL
/* an operation with locking of the memory location by the "wait" lock */
#define _MAS_MODE_LOAD_OP_WAIT           7UL
/* speculative operation with memory lock check */
#define _MAS_MODE_LOAD_OP_SPEC_LOCK_CHECK 7UL

/* an operation with looking of the disambiguation memory table */
#define _MAS_MODE_DAM_LOAD                7UL

/* LOAD (non-speculative) MAS modes for channels 2 and 5. */

#define MAS_MODE_LOAD_OPERATION           0UL /* an operation */
#define MAS_MODE_LOAD_PA                  1UL /* reading by physical address */
/* conditional operation depending on the "check" lock state  */
#define MAS_MODE_LOAD_OP_CHECK            2UL

/* an operation with memory location depending on the "check" lock state and
 *  memory location unlocking */
#define MAS_MODE_LOAD_OP_UNLOCK           3UL
#define MAS_MODE_LOAD_OP_SPEC             3UL /* speculative load */
#define MAS_MODE_FILL_OP                  4UL /* fill operation */
/* semi-speculative operation with memory lock check */
#define MAS_MODE_LOAD_OP_LOCK_CHECK       4UL
#define MAS_MODE_LOAD_RESERVED3           5UL /* reserved */
#define MAS_MODE_LOAD_IOPAGE              6UL /* I/O page access operation */
#define MAS_MODE_LOAD_MMU_AAU_SPEC        7UL /* special MMU or AAU operation*/
/* speculative operation with memory lock check */
#define MAS_MODE_LOAD_OP_SPEC_LOCK_CHECK  7UL

/* STORE MAS modes for channels 2 and 5. */

#define MAS_MODE_STORE_OPERATION          0UL /* an operation */
#define MAS_MODE_STORE_PA                 1UL /* writing by physical address */
/* conditional operation depending on the "wait" lock state with
 * memory location unlocking */
#define MAS_MODE_STORE_OP_WAIT            2UL
/* an operation with memory location unlocking */
#define MAS_MODE_STORE_OP_UNLOCK          3U
#define MAS_MODE_STORE_RESERVED2          4UL
/* same as MAS_MODE_STORE_OP_UNLOCK but no operation performed */
#define MAS_MODE_STORE_NOP_UNLOCK         5UL
#define MAS_MODE_STORE_IOPAGE             6UL /* I/O page access operation */
#define MAS_MODE_STORE_MMU_AAU_SPEC       7UL /* special MMU or AAU operation */

/* MAS "endian"-ness */

#define MAS_ENDIAN_LITTLE                 0UL
#define MAS_ENDIAN_BIG                    1UL

/* MAS non-aligned access switch */

#define MAS_ALIGNED_ADDR                  0UL
#define MAS_NONALIGNED_ADDR               1UL

/* MAS cache enablers */

#define MAS_CACHE_12E_V_E             0UL /* virtual, all caches enabled */
/* physical, DCACHE2,ECACHE enabled only */
#define MAS_CACHE_2E_P_E              0UL
#define MAS_CACHE_2E_E                1UL /* DCACHE2 and ECACHE enabled only*/
#define MAS_CACHE_E_E                 2UL /* ECACHE enabled only */
#define MAS_CACHE_N_E                 3UL /* nothing enabled */

/* MAS cache disablers */

#define MAS_CACHE_N_V_D               0UL /* virtual, nothing disabled */
#define MAS_CACHE_1_P_D               0UL /* physical, DCACHE1 disabled only */
#define MAS_CACHE_1_D                 1UL /* DCACHE1 disabled only */
#define MAS_CACHE_12_D                2UL /* DCACHE1 and DCACHE2 disabled */
#define MAS_CACHE_ALL_D               3UL /* all caches disabled */

/*
 * MAS OPCs
 */

/* mandatory group */
#define MAS_OPC_CACHE_FLUSH          0UL /* Cache(s) flush operations */
 /* Data cache(s) line flush operations */
#define MAS_OPC_DCACHE_LINE_FLUSH    1UL
/* Instruction cache(s) line flush operations*/
#define MAS_OPC_ICACHE_LINE_FLUSH    2UL
#define MAS_OPC_TLB_PAGE_FLUSH       2UL /* TLB page flush operations */
#define MAS_OPC_RESERVED1            3UL
/* Instruction cache(s) flush operations */
#define MAS_OPC_ICACHE_FLUSH         4UL
#define MAS_OPC_TLB_FLUSH            4UL /* TLB flush operations */

#define MAS_OPC_TLB_ADDR_PROBE       5UL /* TLB address probe operations */
#define MAS_OPC_TLB_ENTRY_PROBE      6UL /* TLB entry probe operations */
#define MAS_OPC_AAU_REG              7UL /* AAU registers access */

/* optional group */
#define MAS_OPC_MMU_REG              8UL /* MMU registers access */
#define MAS_OPC_DTLB_REG             9UL /* DTLB registers access */
#define MAS_OPC_L1_REG               10UL /* L1 cache registers access */
#define MAS_OPC_L2_REG               11UL /* L2 cache registers access */

#define MAS_OPC_ICACHE_REG           12UL /* ICACHE registers access */
#define MAS_OPC_ITLB_REG             12UL /* ITLB registers access */

#define MAS_OPC_DAM_REG              13UL /* DAM register(s) access */
#define MAS_OPC_MLT_REG              13UL /* MLT register(s) access */
#define MAS_OPC_CLW_REG              13UL /* CLW register(s) access */
#define MAS_OPC_SNOOP_REG            13UL /* SNOOP register(s) access */

/* PCS (Procedure Chain Stack) registers operations*/
#define MAS_OPC_PCS_REG              14UL
#define MAS_OPC_RESERVED2            15UL


/* Popular complex MASes for some special Linux/E2K situations */
#if 0
#define MAS_BYPASS_L1_CACHE    (MAS_CACHE_1_D << MAS_DCACHE_SHIFT)
#define MAS_BYPASS_ALL_CACHES  (MAS_CACHE_ALL_D << MAS_DCACHE_SHIFT)
#define MAS_NONALIGNED         (MAS_NONALIGNED_ADDR << MAS_NONALIGNED_SHIFT)
#endif
#define MAS_IOADDR             (MAS_MODE_STORE_IOPAGE << MAS_MOD_SHIFT)
#if 0
#define MAS_BIGENDIAN          (MAS_ENDIAN_BIG << MAS_ENDIAN_SHIFT)
#define MAS_FILL_OPERATION     (MAS_MODE_FILL_OP << MAS_MOD_SHIFT)
#define MAS_LOAD_PA            (MAS_MODE_LOAD_PA << MAS_MOD_SHIFT)
#define MAS_LOAD_SPEC          (MAS_MODE_LOAD_OP_SPEC << MAS_MOD_SHIFT)
#define MAS_STORE_PA           (MAS_MODE_STORE_PA << MAS_MOD_SHIFT)
#endif
/* CACHE(s) flush */
#define MAS_CACHE_FLUSH ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_CACHE_FLUSH << MAS_OPC_SHIFT) & MAS_OPC_MASK))

/* DCACHE line flush */
#define MAS_DCACHE_LINE_FLUSH ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_DCACHE_LINE_FLUSH << MAS_OPC_SHIFT) & MAS_OPC_MASK))

/* DCACHE L1 registers access */
#define MAS_DCACHE_L1_REG ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_L1_REG << MAS_OPC_SHIFT) & MAS_OPC_MASK))

/* DCACHE L2 registers access */
#define MAS_DCACHE_L2_REG ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_L2_REG << MAS_OPC_SHIFT) & MAS_OPC_MASK))

/* ICACHE line and DTLB page flush */
#define MAS_ICACHE_LINE_FLUSH ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_ICACHE_LINE_FLUSH << MAS_OPC_SHIFT) & MAS_OPC_MASK))

#define MAS_TLB_PAGE_FLUSH ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_TLB_PAGE_FLUSH << MAS_OPC_SHIFT) & MAS_OPC_MASK))

/* ICACHE and TLB flush */
#define MAS_ICACHE_FLUSH ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_ICACHE_FLUSH << MAS_OPC_SHIFT) & MAS_OPC_MASK))

#define MAS_TLB_FLUSH ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_TLB_FLUSH << MAS_OPC_SHIFT) & MAS_OPC_MASK))

/* MMU registers access */
#define MAS_MMU_REG ((MAS_MODE_STORE_MMU_AAU_SPEC << MAS_MOD_SHIFT) | \
			((MAS_OPC_MMU_REG << MAS_OPC_SHIFT) & MAS_OPC_MASK))

#endif /* __E2K_MAS_H_ */
