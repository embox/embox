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
#error "set MMU_LEVELS"
#endif

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

/**
 * Switches on virtual mode in system.
 */
extern void mmu_on(void);

/**
 * Switches off virtual mode in system. Switches on real mode.
 */
extern void mmu_off(void);

extern mmu_vaddr_t mmu_get_fault_address(void);

/**
 * Allocates new pgd table.
 *
 * @return new MMU context
 * @retval new MMU context on success
 * @retval -1 on failed
 */
extern mmu_ctx_t mmu_create_context(uintptr_t *pgd);

/**
 * Delete pgd and all nested table from mmu tables
 *
 * @param ctx MMU context has been created @link #mmu_create_context() @endlink
 */
extern void mmu_delete_context(mmu_ctx_t ctx);

extern void mmu_set_context(mmu_ctx_t ctx);

/**
 * Get root pointer of context
 *
 * @param ctx - context
 * @retval pointer to first pgd of memory
 */
extern uintptr_t *mmu_get_root(mmu_ctx_t ctx);

extern uintptr_t *mmu_get(int lvl, uintptr_t *entry);
extern void mmu_set(int lvl, uintptr_t *entry, uintptr_t value);
extern void mmu_unset(int lvl, uintptr_t *entry);
extern int mmu_present(int lvl, uintptr_t *entry);

extern uintptr_t mmu_pte_pack(uintptr_t addr, int flags);
extern int mmu_pte_set(uintptr_t *entry, uintptr_t value);
extern uintptr_t mmu_pte_get(uintptr_t *entry);
extern uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags);

extern void mmu_pte_set_writable(uintptr_t *pte, int value);
extern void mmu_pte_set_cacheable(uintptr_t *pte, int value);
extern void mmu_pte_set_usermode(uintptr_t *pte, int value);
extern void mmu_pte_set_executable(uintptr_t *pte, int val);

extern void mmu_flush_tlb(void);

#endif /* MMU_LEVELS > 0 */

#endif /* HAL_MMU_H_ */
