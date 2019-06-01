/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */

#ifndef HAL_MMU_H_
#define HAL_MMU_H_

#include <module/embox/arch/mmu.h>

#ifndef MMU_LEVELS
#define MMU_LEVELS 3
#endif

#define MMU_PAGE_SIZE     (4096)
#define MMU_PAGE_MASK     (4095)

#if MMU_LEVELS > 0
typedef __mmu_paddr_t mmu_paddr_t;
typedef __mmu_vaddr_t mmu_vaddr_t;

typedef __mmu_ctx_t mmu_ctx_t;

typedef __mmu_reg_t mmu_reg_t;

#define PRIxMMUREG      __PRIxMMUREG

struct mmuinfo_regs {
	char mmu_reg_name[0x20];
	mmu_reg_t mmu_reg_value;
};

extern int arch_mmu_get_regs_table_size(void);
extern int
arch_mmu_get_regs_table(struct mmuinfo_regs *buf, int buf_size, int offset);

extern void mmu_on(void);
extern void mmu_off(void);

extern mmu_vaddr_t mmu_get_fault_address(void);

extern mmu_ctx_t mmu_create_context(uintptr_t *pgd);
extern void mmu_set_context(mmu_ctx_t ctx);

extern uintptr_t *mmu_get_root(mmu_ctx_t ctx);
extern uintptr_t *mmu_value(int lvl, uintptr_t *entry);
extern void mmu_set(int lvl, uintptr_t *entry, uintptr_t value);
extern void mmu_unset(int lvl, uintptr_t *entry);
extern int mmu_present(int lvl, uintptr_t *entry);

extern void mmu_pte_set_writable(uintptr_t *pte, int value);
extern void mmu_pte_set_cacheable(uintptr_t *pte, int value);
extern void mmu_pte_set_usermode(uintptr_t *pte, int value);
extern void mmu_pte_set_executable(uintptr_t *pte, int val);

extern void mmu_flush_tlb(void);

#endif /* MMU_LEVELS > 0 */
#endif /* HAL_MMU_H_ */
