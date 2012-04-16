/**
 * @file
 * @brief Implements mapping interface for X86 architecture.
 *
 * @date 15.03.12
 * @author Gleb Efimov
 */

#include <mem/pagealloc/opallocator.h>

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_types.h>

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, mmu_page_flags_t flags) {
	return 0;
}
