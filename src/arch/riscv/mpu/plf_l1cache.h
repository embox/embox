
#ifndef PLATFORM_L1CACHE_SCR4_H
#define PLATFORM_L1CACHE_SCR4_H

#include <stdint.h>
#include <asm/csr.h> // csr_write


#define FLASH_START (0x00000000)
#define FLASH_SIZE  (4 * 1024 * 1024)
#define FLASH_MASK  (~(FLASH_SIZE - 1)  & 0xFFFFFFFF)

#define RAM_START   (0x20000000)
#define RAM_SIZE    (1 * 1024 * 1024)
#define RAM_MASK    (~(RAM_SIZE - 1)    & 0xFFFFFFFF)

#define AHB_START   (0x40000000)
#define AHB_SIZE    (32 * 1024 * 1024)
#define AHB_MASK    (~(AHB_SIZE - 1)    & 0xFFFFFFFF)

#define APB_START   (0x50000000)
#define APB_SIZE    (64 * 1024 * 1024)
#define APB_MASK    (~(APB_SIZE - 1)    & 0xFFFFFFFF)

#define TCM_START   (0xF0000000)
#define TCM_SIZE    (32 * 1024)
#define TCM_MASK    (~(TCM_SIZE - 1)    & 0xFFFFFFFF)

#define PLIC_START  (0xFE000000)
#define PLIC_SIZE   (16 * 1024 * 1024)
#define PLIC_MASK   (~(PLIC_SIZE - 1)   & 0xFFFFFFFF)

#define MTIMER_START  (0xF0040000)
#define MTIMER_SIZE   (32 * 1024)
#define MTIMER_MASK   (~(MTIMER_SIZE - 1)   & 0xFFFFFFFF)

#ifndef   SCR_CSR_MPU_BASE
  #define SCR_CSR_MPU_BASE          0xBC4
#endif
#define   SCR_CSR_MEM_CTRL_GLOBAL   0xBD4

// SCR4 External Architecture Specification v1.0.0, 2023-06-08
// 3.3.4.21. MEM_CTRL_GLOBAL Register [0xBD4] - Memory Regions Control Global Register
// Bits  Name    Access  Reset Description
// 0     L1ICE   RW      0     Instruction cache L1 enable
// 1     L1DCE   RW      0     Data cache L1 enable
// 2     L1ICF   RSW1S   0     Instruction cache L1 flush control.
//                             Setting to 1 starts the cache flush/invalidate procedure.
//                             For read access, defines status of the procedure:
//                             0 - cache flush/invalidate is complete
//                             1 - cache flush/invalidate is in progress
// 3     L1DCF   RSW1S   0     Data cache L1 flush control.
//                             Setting to 1 starts the cache flush/invalidate procedure.
//                             For read access, defines status of the procedure:
//                             0 - cache flush/invalidate is complete
//                             1 - cache flush/invalidate is in progress
#define MEM_CTRL_GLOBAL_L1DCF_Pos           (3)
#define MEM_CTRL_GLOBAL_L1ICF_Pos           (2)
#define MEM_CTRL_GLOBAL_L1DCE_Pos           (1)
#define MEM_CTRL_GLOBAL_L1ICE_Pos           (0)

#define MEM_CTRL_GLOBAL_L1DCF_Msk           (1 << MEM_CTRL_GLOBAL_L1DCF_Pos)
#define MEM_CTRL_GLOBAL_L1ICF_Msk           (1 << MEM_CTRL_GLOBAL_L1ICF_Pos)
#define MEM_CTRL_GLOBAL_L1DCE_Msk           (1 << MEM_CTRL_GLOBAL_L1DCE_Pos)
#define MEM_CTRL_GLOBAL_L1ICE_Msk           (1 << MEM_CTRL_GLOBAL_L1ICE_Pos)


#define MEM_CTRL_GLOBAL_ENABLE_CACHES       (MEM_CTRL_GLOBAL_L1DCE_Msk | \
                                             MEM_CTRL_GLOBAL_L1ICE_Msk   )

#define MEM_CTRL_GLOBAL_SAFE_ENABLE_CACHES  (MEM_CTRL_GLOBAL_L1DCF_Msk | \
                                             MEM_CTRL_GLOBAL_L1ICF_Msk | \
                                             MEM_CTRL_GLOBAL_L1DCE_Msk | \
                                             MEM_CTRL_GLOBAL_L1ICE_Msk   )

#define SCR_CSR_MPU_SEL           (SCR_CSR_MPU_BASE + 0)
#define SCR_CSR_MPU_CTRL          (SCR_CSR_MPU_BASE + 1)
#define SCR_CSR_MPU_ADDR          (SCR_CSR_MPU_BASE + 2)
#define SCR_CSR_MPU_MASK          (SCR_CSR_MPU_BASE + 3)

// MPU_SELECT
// ----------------------------------------------------------------------
// |Bits  |Name |Access|Reset  |Description                              |
// ----------------------------------------------------------------------
// |3:0   |ENTRY|RW    |0      |Selected entry: 0..15                    |
// |31:4  |RSV  |RZ    |0      |Reserved                                 |

// MPU_CONTROL Register (RSTVAL = (ENTRY == 0) ? 1 : 0;)
// ----------------------------------------------------------------------
// |Bits  |Name |Access|Reset  |Description                              |
// ----------------------------------------------------------------------
// |0     |VALID|RW    |RSTVAL |Current entry is enabled                 |
// |1     |MMR  |RW    |RSTVAL |M-Mode reads are permitted               |
// |2     |MMW  |RW    |RSTVAL |M-Mode writes are permitted              |
// |3     |MMX  |RW    |RSTVAL |M-Mode execution is permitted            |
// |4     |UMR  |RW    |0      |U-Mode reads are permitted               |
// |5     |UMW  |RW    |0      |U-Mode writes are permitted              |
// |6     |UMX  |RW    |0      |U-Mode execution is permitted            |
// |15..7 |RSV  |RZ    |0      |Reserved                                 |
// |17:16 |MTYPE|RW    |RSTVAL |Memory type:                             |
// |      |     |      |       |0 - cacheable, weakly-ordered            |
// |      |     |      |       |1 - non-cacheable, strong-ordered        |
// |      |     |      |       |2 - non-cacheable, weakly-ordered        |
// |      |     |      |       |3 - CPU configuration memory-mapped I/O, |
// |      |     |      |       |    non-cacheable, strong-ordered        |
// |30..18|RSV  |RZ    |0      |Reserved                                 |
// |31    |LOCK |RW    |0      |Current entry is locked. If "LOCK" field is active, any writes to |
// |      |     |      |       |control or associated Address/Mask registers are ignored.         |
// |      |     |      |       |Entry can only be unlocked with a reset.                          |

// MPU_ADDRESS (ADDRVAL = (ENTRY == 0) ? 0 : undefined;)
// ----------------------------------------------------------------------
// |Bits  |Name |Access|Reset  |Description                              |
// ----------------------------------------------------------------------
// |9:0   |RSV  |RZ    |0      |Reserved                                 |
// |29:10 |ADDR |RW    |ADDRVAL|Address[31:12]                           |
// |31:30 |RSV  |RZ    |0      |Reserved                                 |

// MPU_MASK (MASKVAL = (ENTRY == 0) ? 0 : undefined;)
// ----------------------------------------------------------------------
// |Bits  |Name |Access|Reset  |Description                              |
// ----------------------------------------------------------------------
// |9:0   |RSV  |RZ    |0      |Reserved                                 |
// |29:10 |MASK |RW    |MASKVAL|Mask[31:12]                              |
// |31:30 |RSV  |RZ    |0      |Reserved                                 |


static inline void plf_mpu_common_entry_init(
  uint8_t entry, uint32_t address, uint32_t mask, uint8_t mtype
) {
  uint32_t mpu_control;

  mpu_control = ((mtype & 0x3) << 16) | 0x7F;
  if (mtype == 3) {
    mpu_control &= ~((1 << 6) | (1 << 5) | (1 << 4) | (1 << 3)); 
  }
  csr_write(SCR_CSR_MPU_SEL,  entry);
  csr_write(SCR_CSR_MPU_ADDR, (address >> 2));
  csr_write(SCR_CSR_MPU_MASK, (mask    >> 2));
  csr_write(SCR_CSR_MPU_CTRL, mpu_control);
}

static inline void plf_mpu_entry_init(
  uint8_t entry, uint32_t address, uint32_t mask, uint8_t cacheable
) {
  plf_mpu_common_entry_init(entry, address, mask, (~cacheable & 0x1));
}

static inline void plf_mpu_init(uint8_t cacheable_flash, uint8_t cacheable_ram) {
  plf_mpu_common_entry_init(6, MTIMER_START,  MTIMER_MASK,  3);
  plf_mpu_entry_init(5, PLIC_START,  PLIC_MASK,  0);
  //plf_mpu_entry_init(4, TCM_START,   TCM_MASK,   0);
  plf_mpu_entry_init(3, APB_START,   APB_MASK,   0);
  plf_mpu_entry_init(2, AHB_START,   AHB_MASK,   0);
  plf_mpu_entry_init(1, RAM_START,   RAM_MASK,   cacheable_ram);
  plf_mpu_entry_init(0, FLASH_START, FLASH_MASK, cacheable_flash);
}

// Important! By default all area is marked as non-cacheable (entry 0 of MPU).
//            It is need to remap area (to add cacheable and non-cacheable subareas)
//            and to disable/overwrite entry 0 of MPU.
// Note: Cache in SCR4-min is flushed by default
static inline void plf_l1cache_init() {
  // 7.1.4.1. Enabling and Disabling the L1 Caches
  // The setting MEM_CTRL_GLOBAL[L1ICE] or MEM_CTRL_GLOBAL[L1DCE] = 1 enables
  // corresponded L1 cache. After this the cache executes flush-invalidating.
  // During this procedure, the cache couldn’t be accessed for a cacheable transaction.
  // The non-cacheable transaction will be bypassed to memory subsystem.
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, MEM_CTRL_GLOBAL_ENABLE_CACHES);
}


static inline void enable_l1_caches() {
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, MEM_CTRL_GLOBAL_SAFE_ENABLE_CACHES);
}

static inline void enable_l1i_cache() {
  uint32_t value;

  // Set L1ICE and L1ICF bits:
  value  = csr_read(SCR_CSR_MEM_CTRL_GLOBAL);
  value |= (MEM_CTRL_GLOBAL_L1ICF_Msk | MEM_CTRL_GLOBAL_L1ICE_Msk);
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, value);
}

static inline void enable_l1d_cache() {
  uint32_t value;

  // Set L1DCE and L1DCF bits:
  value  = csr_read(SCR_CSR_MEM_CTRL_GLOBAL);
  value |= (MEM_CTRL_GLOBAL_L1DCF_Msk | MEM_CTRL_GLOBAL_L1DCE_Msk);
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, value);
}

static inline void disable_l1_caches() {
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, 0);
}

static inline void disable_l1i_cache() {
  uint32_t value;

  // Left only previous L1DCE value
  value  = csr_read(SCR_CSR_MEM_CTRL_GLOBAL);
  value &= MEM_CTRL_GLOBAL_L1DCE_Msk;
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, value);
}

static inline void disable_l1d_cache() {
  uint32_t value;

  // Left only previous L1ICE value
  value  = csr_read(SCR_CSR_MEM_CTRL_GLOBAL);
  value &= MEM_CTRL_GLOBAL_L1ICE_Msk;
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, value);
}


static inline void flush_l1i_cache() {
  uint32_t value;

  // Left previous L1DCE, L1ICE values and set L1ICF bit
  value  = csr_read(SCR_CSR_MEM_CTRL_GLOBAL);
  value |= MEM_CTRL_GLOBAL_L1ICF_Msk;
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, value);
}

static inline void flush_l1d_cache() {
  uint32_t value;

  // Left previous L1DCE, L1ICE values and set L1DCF bit
  value  = csr_read(SCR_CSR_MEM_CTRL_GLOBAL);
  value |= MEM_CTRL_GLOBAL_L1DCF_Msk;
  csr_write(SCR_CSR_MEM_CTRL_GLOBAL, value);
}


#endif // PLATFORM_L1CACHE_SCR4_H