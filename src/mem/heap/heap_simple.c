/**
 * @file
 * @brief Simple memory allocator
 *
 * @date 13.10.09
 * @author Nikolay Korotky -- initial implementation
 * @author Denis Deryugin -- rework
 */

#include <string.h>
#include <stdlib.h>
#include <embox/unit.h>
#include <mem/heap.h>
#include <mem/page.h>
#include <kernel/printk.h>
#include <util/binalign.h>
#include <util/log.h>

static int simple_heap_intialized = 0;
static void *heap_simple_start;
static void *heap_simple_end;

#define HEAP_SIZE OPTION_MODULE_GET(embox__mem__heap_api,NUMBER,heap_size)
#define HEAP_ALIGN OPTION_MODULE_GET(embox__mem__heap_api,NUMBER,align)
#define ALIGN_PADDING sizeof(size_t)

#define MEM_POOL_SIZE  (HEAP_SIZE / PAGE_SIZE())
static void *mem_pool;

struct mem_control_block {
	int is_available;
	size_t size; /* Length of memory chunk WITHOUT this structure
	              * (which is located at the beginning of the chunk) */
};

#define MCB_SZ binalign_bound(sizeof(struct mem_control_block), HEAP_ALIGN)

static int malloc_init(void) {
	extern struct page_allocator *__heap_pgallocator;
	struct mem_control_block *init_mcb;

	mem_pool = page_alloc(__heap_pgallocator, MEM_POOL_SIZE);
	if (NULL == mem_pool) {
		return -1;
	}

	if (!binalign_check_bound((uintptr_t) mem_pool, HEAP_ALIGN)) {
		log_error("Page allocator is unaligned!");
		page_free(__heap_pgallocator, mem_pool, MEM_POOL_SIZE);
		return -1;
	}

	heap_simple_start = mem_pool;
	heap_simple_end   = heap_simple_start + HEAP_SIZE;

	simple_heap_intialized = 1;

	init_mcb = (struct mem_control_block *) heap_simple_start;
	init_mcb->is_available = 1;
	init_mcb->size = HEAP_SIZE - MCB_SZ;

	return 0;
}

static void _mem_defrag(void) {
	void *ptr;
	struct mem_control_block *mcb, *next_mcb;

	ptr = heap_simple_start;

	while (ptr < heap_simple_end) {
		mcb = (struct mem_control_block *) ptr;
		if (mcb->is_available == 1) {
			next_mcb = (struct mem_control_block *) (ptr + mcb->size + MCB_SZ);

			if ((void *) next_mcb < (void *) heap_simple_end &&
					next_mcb->is_available == 1) {
				mcb->size += next_mcb->size + MCB_SZ;
			}
		}

		ptr += mcb->size + MCB_SZ;
	}
}

void free(void *ptr) {
	struct mem_control_block *mcb;
	size_t shift = *(int*)(ptr - ALIGN_PADDING);
	ptr -= MCB_SZ + shift + ALIGN_PADDING;

	mcb = ptr;
	mcb->is_available = 1;

	 _mem_defrag();
	return;
}

void *malloc(size_t size) {
	return memalign(sizeof(void*), size);
}

void *memalign(size_t boundary, size_t size) {
	void *ptr;
	struct mem_control_block *mcb, *next_mcb;
	size_t shift, total_mem_size;

	if (size == 0 || (boundary & (boundary - 1)) != 0) {
		return NULL;
	}

	if (!simple_heap_intialized) {
		if (-1 == malloc_init()) {
			log_error("Failed to initialize heap!\n");
			return NULL;
		}
	}

	ptr = heap_simple_start;
	size = binalign_bound(size, HEAP_ALIGN);

	while (ptr + MCB_SZ + size < heap_simple_end) {
		mcb = ptr;
		shift = (boundary - ((int)(ptr + MCB_SZ + ALIGN_PADDING) % boundary)) % boundary;
		total_mem_size = size + ALIGN_PADDING + shift + MCB_SZ;

		if (mcb->is_available && mcb->size >= total_mem_size - MCB_SZ) {
			mcb->is_available = 0;

			if (mcb->size > total_mem_size) {
				next_mcb = ptr + total_mem_size;

				next_mcb->size = mcb->size - total_mem_size;
				next_mcb->is_available = 1;

				mcb->size = total_mem_size - MCB_SZ;
			}
			*(int*)(ptr + MCB_SZ + shift) = shift;
			return ptr + total_mem_size - size;
		}

		ptr += mcb->size + MCB_SZ;
	}

	return 0;
}

void *calloc(size_t nmemb, size_t size) {
	void *tmp = malloc(nmemb * size);
	memset(tmp, 0, nmemb * size);
	return tmp;
}

void *realloc(void *ptr, size_t size) {
	struct mem_control_block *mcb;
	char *tmp = malloc(size);

	if (ptr == NULL) {
		return tmp;
	}

	mcb = ptr - MCB_SZ;
	memcpy(tmp, ptr, mcb->size);
	free(ptr);

	return tmp;
}
