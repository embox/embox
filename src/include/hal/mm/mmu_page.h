/**
 * @file
 * @brief Interface for working with MMU table records.
 *
 * @details Implements interface for working with different types of MMU flag
 *         such as @link #MMU_PAGE_WRITEABLE @endlink. You can mark page
 *         invalid it means if system call this page it impels page_fault trap.
 *         IMPORTENT: These functions work with already mapped pages only!
 *
 * @date 07.04.2010
 * @author Anton Bondarev
 */

#ifndef MMU_PAGE_H_
#define MMU_PAGE_H_

#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_page.h>

/**
 * Set flag MMU_PAGE_WRITEABLE for pointed virtual page
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 */
extern void mmu_page_mark_writable(mmu_ctx_t ctx, vaddr_t vaddr);

/**
 * Set flag MMU_PAGE_EXECUTEABLE for pointed virtual page
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 */
extern void mmu_page_mark_executeable(mmu_ctx_t ctx, vaddr_t vaddr);

/**
 * Set flag MMU_PAGE_CACHEABLE for pointed virtual page
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 */
extern void mmu_page_mark_cacheable(mmu_ctx_t ctx, vaddr_t vaddr);

/**
 * Received MMU flags for pointed page
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 *
 * @return All MMU flags for pointer page *
 */
extern mmu_page_flags_t mmu_page_get_flags(mmu_ctx_t ctx, vaddr_t vaddr);

/**
 * Set all MMU flags for pointed page in MMU table record
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 *
 * @return All MMU flags for pointer page
 */
extern void mmu_page_set_flags(mmu_ctx_t ctx, vaddr_t vaddr, mmu_page_flags_t flags);

/**
 * Marks pointed page as valid. It means that we set valid flag for this page
 * and if it's necessary cache this page in hardware MMU table. After this call
 * the system can access to this page without page_fault trap.
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 */
extern void mmu_page_mark_valid(mmu_ctx_t ctx, vaddr_t vaddr);

/**
 * Marks pointed page as invalid. It means that we clear valid flag for this
 * page. After this call if the system try to access to this page the hardware
 * impels page_fault trap.
 *
 * @param ctx MMU context contained virtual page
 * @param vaddr virtual address for mapped page
 */
extern void mmu_page_mark_invalide(mmu_ctx_t ctx, vaddr_t vaddr);

#endif /* MMU_PAGE_H_ */
