#include <asm/asm.h>
#include <framework/mod/options.h>

.section .text
.balign 256
.global vector_table

vector_table:
    j riscv_trap_handler        /* _panic_handler */            /* 0: Exception entry */
    j riscv_trap_handler        /* 1: Free interrupt number */
    j riscv_trap_handler        /* 2: Free interrupt number */
    j riscv_trap_handler        /* 3: Free interrupt number */
    j riscv_trap_handler        /* 4: Free interrupt number */
    j riscv_trap_handler        /* 5: Free interrupt number */
    j riscv_trap_handler        /* 6: Free interrupt number */
    j riscv_trap_handler        /* 7: Free interrupt number */
    j riscv_trap_handler        /* 8: Free interrupt number */
    j riscv_trap_handler        /* 9: Free interrupt number */
    j riscv_trap_handler        /* 10: Free interrupt number */
    j riscv_trap_handler        /* 11: Free interrupt number */
    j riscv_trap_handler        /* 12: Free interrupt number */
    j riscv_trap_handler        /* 13: Free interrupt number */
    j riscv_trap_handler        /* 14: Free interrupt number */
    j riscv_trap_handler        /* 15: Free interrupt number */
    j riscv_trap_handler        /* 16: Free interrupt number */
    j riscv_trap_handler        /* 17: Free interrupt number */
    j riscv_trap_handler        /* 18: Free interrupt number */
    j riscv_trap_handler        /* 19: Free interrupt number */
    j riscv_trap_handler        /* 20: Free interrupt number */
    j riscv_trap_handler        /* 21: Free interrupt number */
    j riscv_trap_handler        /* 22: Free interrupt number */
    j riscv_trap_handler        /* 23: Free interrupt number */
    j riscv_trap_handler        /* _panic_handler */           /* 24: ETS_INT_WDT_INUM panic-interrupt (soc-level panic) */
    j riscv_trap_handler        /* _panic_handler */           /* 25: ETS_CACHEERR_INUM panic-interrupt (soc-level panic) */
    j riscv_trap_handler        /* MEMPROT_ISR */               /* 26: ETS_MEMPROT_ERR_INUM handler (soc-level panic) */
    j riscv_trap_handler        /* ASTDBG_ISR */        /* 27: ETS_ASSIST_DEBUG_INUM handler (soc-level panic) */
    j riscv_trap_handler        /* 28: Free interrupt number */
    j riscv_trap_handler        /* 29: Free interrupt number */
    j riscv_trap_handler        /* 30: Free interrupt number */
    j riscv_trap_handler        /* 31: Free interrupt number */

    .size vector_table, .-vector_table