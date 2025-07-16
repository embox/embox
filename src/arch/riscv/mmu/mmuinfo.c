/**
 * @file
 *
 * @date 15.09.2024
 * @author Suraj Sonawane
 */

#include <string.h>
#include <stdint.h> 
#include <lib/libds/array.h>
#include <util/math.h>
#include <hal/mmu.h>
#include <asm/regs.h> 


struct mmuinfo_reg_access {
    const char *reg_name;
    uint64_t (*mmureg_getter)(void);
};

static uint64_t get_satp(void) {
    return read_csr(satp);  // SATP register controls MMU configuration
}

static uint64_t get_stval(void) {
    return read_csr(stval);  // STVAL holds the faulting virtual address
}

static uint64_t get_sstatus(void) {
    return read_csr(sstatus); // SSTATUS contains the status of the supervisor
}

static const struct mmuinfo_reg_access mmuinfo_regs[] = {
    {"SATP", get_satp},
    {"STVAL", get_stval},
    {"SSTATUS", get_sstatus}
};

int arch_mmu_get_regs_table_size(void) {
    return ARRAY_SIZE(mmuinfo_regs);
}

int arch_mmu_get_regs_table(struct mmuinfo_regs *buf, int buf_size, int offset) {
    int cnt;
    int i;

    cnt = ARRAY_SIZE(mmuinfo_regs) - offset;
    if (cnt <= 0) {
        return 0;
    }
    cnt = min(buf_size, cnt);

    for (i = 0; i < cnt; i++) {
        strncpy(buf[i].mmu_reg_name, mmuinfo_regs[offset + i].reg_name, sizeof(buf[i].mmu_reg_name));
        buf[i].mmu_reg_name[sizeof(buf[i].mmu_reg_name) - 1] = '\0';

        buf[i].mmu_reg_value = mmuinfo_regs[offset + i].mmureg_getter();
    }

    return cnt;
}
