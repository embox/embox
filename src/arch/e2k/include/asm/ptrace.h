/**
 * @file
 *
 * @date Mar 16, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_
#define SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <e2k_api.h>

#define RESTORE_COMMON_REGS(regs)                   \
({                                  \
    uint64_t ctpr1 = regs->ctpr1, ctpr2 = regs->ctpr2,   \
        ctpr3 = regs->ctpr3, lsr = regs->lsr,       \
        ilcr = regs->ilcr;                  \
    /* ctpr2 is restored first because of tight time constraints    \
     * on restoring ctpr2 and aaldv. */             \
    E2K_SET_DSREG(ctpr2, ctpr2);                    \
    E2K_SET_DSREG(ctpr1, ctpr1);                    \
    E2K_SET_DSREG(ctpr3, ctpr3);                    \
    E2K_SET_DSREG(lsr, lsr);                    \
    E2K_SET_DSREG(ilcr, ilcr);                  \
})

struct pt_regs {
	uint64_t lsr;
	uint64_t ilcr;
	uint64_t ctpr1;
	uint64_t ctpr2;
	uint64_t ctpr3;

	uint64_t dr0;

	uint64_t cr0_hi;
	uint64_t cr1_lo;
	uint64_t cr1_hi;
	uint64_t pcsp_hi;
	uint64_t psp_hi;
	uint64_t usd_lo;
	uint64_t usd_hi;
};

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->dr0 = retcode;
}

#endif /* __ASSEMBLER__ */

#define E2K_PT_REGS_SIZE (8 * (5 + 1 + 7))

/* Offsets of registers fields within struct pt_regs */
#define E2K_PT_REGS_LSR      (0 * 8)
#define E2K_PT_REGS_ILCR     (1 * 8) /* 8 */
#define E2K_PT_REGS_CTRP1    (2 * 8) /* 16 */
#define E2K_PT_REGS_CTRP2    (3 * 8) /* 24 */
#define E2K_PT_REGS_CTRP3    (4 * 8) /* 32 */
#define E2K_PT_REGS_DR0      (5 * 8) /* 40 */

#define E2K_PT_REGS_CR0_HI   (6 * 8) /* 0x00 */
#define E2K_PT_REGS_CR1_LO   (7 * 8) /* 0x08 */
#define E2K_PT_REGS_CR1_HI   (8 * 8) /* 0x10 */
#define E2K_PT_REGS_PCSP_HI  (9 * 8) /* 0x18 */
#define E2K_PT_REGS_PSP_HI   (10 * 8) /* 0x20 */
#define E2K_PT_REGS_USD_LO   (11 * 8) /* 0x28 */
#define E2K_PT_REGS_USD_HI   (12 * 8) /* 0x30 */

#endif /* SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_ */
