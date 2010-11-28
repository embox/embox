/**
 * @file
 *
 * @date 07.04.2010
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_MMU_PAGE_H_
#define MICROBLAZE_MMU_PAGE_H_

#include <hal/mm/mmu_core.h>

inline static void mmu_page_mark_writable(mmu_ctx_t ctx, vaddr_t vaddr) {

}

inline static void mmu_page_mark_executeable(mmu_ctx_t ctx, vaddr_t vaddr) {

}

inline static void mmu_page_mark_cacheable(mmu_ctx_t ctx, vaddr_t vaddr) {

}

inline static mmu_page_flags_t mmu_page_get_flags(mmu_ctx_t ctx, vaddr_t vaddr) {
	return 0;
}

inline static void mmu_page_set_flags(mmu_ctx_t ctx, vaddr_t vaddr,
						mmu_page_flags_t flags) {

}

inline static void mmu_page_mark_valid(mmu_ctx_t ctx, vaddr_t vaddr) {

}

inline static void mmu_page_mark_invalide(mmu_ctx_t ctx, vaddr_t vaddr) {

}

#endif /* MICROBLAZE_MMU_PAGE_H_ */
