/**
 * @file
 * @brief RISC-V Floating Point Unit (FPU) Management Module
 *
 * @author Aleksey Zhmulin
 *         - Initial implementation
 * @author Suraj Sonawane
 *         - Added FPU context initialization and disable functions
 * @date 21.08.23
 */

#include <asm/csr.h>
#include <embox/unit.h>
#include <string.h>

#define FPU_REGISTERS 32

typedef struct fpu_context {
    union {
        float s[FPU_REGISTERS]; // Single-precision floating-point registers
        double d[FPU_REGISTERS / 2]; // Double-precision floating-point registers
    } fpu_regs;
} fpu_context_t;

EMBOX_UNIT_INIT(riscv_enable_fpu);

static int riscv_enable_fpu(void) {
	set_csr_bit(mstatus, MSTATUS_FS);
	return 0;
}

/**
 * @brief Initialize the FPU context by clearing it.
 *
 * @param fpu_ctx Pointer to the FPU context to be initialized.
 */
void riscv_fpu_context_init(fpu_context_t *fpu_ctx) {
    memset(fpu_ctx, 0, sizeof(uint32_t) * FPU_REGISTERS);
}

/**
 * @brief Disable the FPU by clearing the FS (Floating-point Status) field in mstatus.
 */
void riscv_fpu_disable(void) {
    clear_csr_bit(mstatus, MSTATUS_FS);
}
