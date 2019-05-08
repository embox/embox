/**
 * @file
 * @brief
 *
 * @date 09.11.12
 * @author Anton Bulychev
 */

#include <util/log.h>

#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>

#include <mem/mmap.h>

#include <kernel/task/resource/mmap.h>

#include <util/dlist.h>

struct marea *mmap_find_marea(struct emmap *mmap, mmu_vaddr_t vaddr) {
	return NULL;
}

struct marea *mmap_find_marea_next(struct emmap *mmap, mmu_vaddr_t vaddr,
		struct marea *prev) {
	return NULL;
}

void mmap_add_marea(struct emmap *mmap, struct marea *marea) {
}

void mmap_del_marea(struct marea *marea) {
}

struct marea *mmap_alloc_marea(struct emmap *mmap, size_t size, uint32_t flags) {
	return NULL;
}

int mmap_mapping(struct emmap *emmap) {
	return 0;
}

int mmap_inherit(struct emmap *mmap, struct emmap *p_mmap) {
	return 0;
}

void mmap_init(struct emmap *mmap) {
}

void mmap_free(struct emmap *mmap) {
}

void mmap_clear(struct emmap *mmap) {
}

struct marea *mmap_place_marea(struct emmap *mmap, uint32_t start, uint32_t end, uint32_t flags) {
	return NULL;
}

int mmap_kernel_init(void) {
	return 0;
}

int mmap_kernel_inited(void) {
	return 0;
}

struct emmap *mmap_early_emmap(void) {
	static struct emmap early_emmap = {
		NULL,
		0,
		DLIST_INIT(early_emmap.marea_list)
	};

	return &early_emmap;
}

int mmap_do_marea_map(struct emmap *mmap, struct marea *marea) {
	return -1;
}

void mmap_do_marea_unmap(struct emmap *mmap, struct marea *marea) {
}

#include <kernel/task/resource/mmap.h>

mmu_ctx_t mmap_get_current_context(void) {
	struct emmap *emmap;

	emmap = task_self_resource_mmap();

	return emmap->ctx;
}

/* Stubs for marea */
struct marea *marea_create(uint32_t start, uint32_t end, uint32_t flags, bool is_allocated) {
	return NULL;
}

void marea_destroy(struct marea *marea) {
}
