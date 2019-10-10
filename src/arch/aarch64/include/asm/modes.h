/**
 * @file modes.h
 * @brief Bits for system registers (DAIF/SPSR/SCTLR/etc)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.07.2019
 */

#ifndef AARCH_ASM_MODES_H_
#define AARCH_ASM_MODES_H_

/* DAIF bits */
#define DAIF_D_BIT 0x200
#define DAIF_A_BIT 0x100
#define DAIF_I_BIT 0x080
#define DAIF_F_BIT 0x040

/* SCTLR bits */
#define SCTLR_M    0x0001 /* MMU enable */
#define SCTLR_A    0x0002 /* Alignment check enable */
#define SCTLR_C    0x0004 /* Data cache enable */
#define SCTLR_SA   0x0008 /* Stack alignment check enable */
#define SCTLR_I    0x1000 /* Instruction cache enable */

/* TCR bits */
#define TCR_TG0_MASK 0x0000C000LL
#define TCR_TG1_MASK 0xC0000000LL

#define TCR_TG0_4KB  (0x0LL << 14)
#define TCR_TG0_64KB (0x1LL << 14)
#define TCR_TG0_16KB (0x2LL << 14)

#endif /* AARCH_ASM_MODES_H_ */
