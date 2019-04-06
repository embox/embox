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
};

#endif /* __ASSEMBLER__ */

#define PT_REGS_SIZE (sizeof(uint64_t) * 5)

/* Offsets of registers fields within struct pt_regs */
#define PT_LSR   0
#define PT_ILCR  8
#define PT_CTRP1 16
#define PT_CTRP2 24
#define PT_CTRP3 32

#endif /* SRC_ARCH_E2K_INCLUDE_ASM_PTRACE_H_ */
