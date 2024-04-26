/**
 * @file
 * @brief Boundary Markers algorithm implementation
 *
 * @date 24.11.2011
 * @author Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <embox/unit.h>
#include <mem/page.h>
#include <mem/heap.h>
#include <mem/heap_afterfree.h>
#include <util/math.h>
#include <util/binalign.h>
#include <kernel/printk.h>
#include <kernel/sched/sched_lock.h>

#define DEBUG 0
#if DEBUG
#define printd(...) printk(__VA_ARGS__)
#else
#define printd(...) do {} while(0);
#endif

struct free_block_link {
	struct free_block_link *prev;
	struct free_block_link *next;
};

struct free_block {
	/* This member must be first. */
	size_t size;                 /**<< Size of block. First two bits of size stores service information about busing
	                                   of current and previous blocks. */
	struct free_block_link link; /**<< Link in global list of free blocks. */
};

struct heap_desc {
	/* This member must be first. */
	struct free_block_link free_blocks; /* List of free blocks */
	int count; /* Count of currently allocated objects */
};

#define get_clear_size(size) ((size) & ~3)
#define get_flags(size) ((size) & 3)

static struct free_block_link *heap_get_free_blocks(void *heap) {
	struct heap_desc *heap_desc = (struct heap_desc *) heap;
	return &heap_desc->free_blocks;
}

static int block_is_busy(struct free_block *block) {
	return block->size & 0x1;
}

static int prev_is_busy(struct free_block *block) {
	return block->size & 0x2;
}

static void block_link(struct free_block_link *free_blocks_list,
		struct free_block *block) {
	block->link.next = free_blocks_list->next;
	block->link.prev = free_blocks_list;
	free_blocks_list->next->prev = &block->link;
	free_blocks_list->next = &block->link;
}

static void block_unlink(struct free_block *block) {
	block->link.next->prev = block->link.prev;
	block->link.prev->next = block->link.next;
}

static void mark_block(struct free_block *block) {
	block->size |= 0x1;
}

static void clear_block(struct free_block *block) {
	block->size &= ~0x1;
}

static void mark_prev(struct free_block *block) {
	block->size |= 0x2;
}

static void mark_next(struct free_block *block) {
	struct free_block *nblock; /* next block */
	size_t size = get_clear_size(block->size);

	nblock = (struct free_block *) ((char *) block + size);
	nblock->size |= 0x2;
}

static void clear_next(struct free_block *block) {
	struct free_block *nblock; /* next block */
	size_t size = get_clear_size(block->size);

	nblock = (struct free_block *) ((char *) block + size);
	nblock->size &= ~0x2;
}

static void set_end_size(struct free_block *block) {
	size_t size = get_clear_size(block->size);

	*(uintptr_t *) ((void *) block + size - sizeof(block->size)) = size;
}

static struct free_block * concatenate_prev(struct free_block *block) {
	size_t prev_size, new_size;
	struct free_block *pblock; /* prev block */

	if (prev_is_busy(block)) {
		return block;
	}

	prev_size = *((uintptr_t *) ((uintptr_t *) block - 1));
	pblock = (struct free_block *) ((char *) block - get_clear_size(prev_size));

	/* Set size for new free block */
	new_size = get_clear_size(prev_size) + get_clear_size(block->size); /* block->size is clear) */
	pblock->size = new_size | 0x2; /* prev is busy */
	set_end_size(pblock);

	/* @c pblock already in list of free blocks. So, simply unlink @c block */
	block_unlink(block);

	return pblock;
}

static struct free_block * concatenate_next(struct free_block *block) {
	size_t size;
	struct free_block *nblock; /* next block */

	size   = get_clear_size(block->size);
	nblock = (struct free_block *) ((char *) block + size);

	if (block_is_busy(nblock)) {
		return block;
	}

	/* Set size for new free block. All flags inherited from @block */
	size += get_clear_size(nblock->size);
	block->size = size | get_flags(block->size);
	set_end_size(block);

	/* @c block already in list of free blocks. So, simply unlink @c nblock */
	block_unlink(nblock);

	return block;
}

static void block_set_size(struct free_block *block, size_t size) {
	uintptr_t flags = get_flags(block->size);
	block->size = flags | size;
}

static struct free_block * cut(struct free_block_link *free_blocks_list,
		struct free_block *block, size_t size) {
	struct free_block *nblock; /* new block */
	size_t offset;

	offset = size + sizeof(block->size);
	offset = binalign_type(offset, struct free_block);
	nblock = (struct free_block *)((char *)block + offset);

	block_unlink(block);
	block_link(free_blocks_list, nblock);

	/* Set size for new free block: in begin and end of block */
	nblock->size = get_clear_size(block->size) - offset;
	set_end_size(nblock);

	/* Set size of new busy block: in begin of block.
	 * Also inherit flags of initial @c block.  */
	block_set_size(block, offset);

	/* Set corresponding flags in busy */
	mark_block(block);
	mark_prev(nblock);

	return block;
}

/* Splits one block in two: free block aligned on @c boundary (of size >= @c size) and remainder.
 * If we cant' split so, do nothing and return NULL. */
static struct free_block *block_align(struct free_block_link *free_blocks_list,
		struct free_block *block, size_t boundary, size_t size) {
	struct free_block *aligned_block;
	size_t aligned_addr;
	size_t aligned_block_size = 1;

	/* If we are already aligning */
	if (binalign_check_bound((size_t)block + sizeof(block->size), boundary)) {
		return ((get_clear_size(block->size) - sizeof(block->size)) >= size) ? block : NULL;
	}

	/* @c block == |remainder|new aligned block|
	 *
	 * Remainder is free block with at least minimum size */
	aligned_addr = binalign_bound((size_t)block + sizeof(struct free_block) +
			2 * sizeof(block->size), boundary);

	/* If aligned address is outer of block */
	if (aligned_addr > (size_t)block + get_clear_size(block->size)) {
		return NULL;
	}

	aligned_block = (struct free_block *) (aligned_addr - sizeof(aligned_addr));

	aligned_block_size = get_clear_size(block->size) - ((size_t) aligned_block - (size_t) block);

	/* If there are no memory to allocate @c size bytes in aligned block than return NULL */
	if (aligned_block_size < size + sizeof(aligned_addr)) {
		return NULL;
	}

	/* So, if we are here, we have aligned block, that can store @c size bytes,
	 * and free remainder. */

	/* Init aligned block */
	aligned_block->size = aligned_block_size;
	set_end_size(aligned_block);
	block_link(free_blocks_list, aligned_block);

	/* Init remainder */
	block_set_size(block, (size_t) aligned_block - (size_t) block);
	set_end_size(block);

	return aligned_block;
}

void *bm_memalign(void *heap, size_t boundary, size_t size) {
	struct free_block *block;
	struct free_block_link *link;
	void *ret_addr;
	struct free_block_link *free_blocks_list;
	struct heap_desc *heap_desc = (struct heap_desc *) heap;

	if (size <= 0) {
		return NULL;
	}

	sched_lock();

	free_blocks_list = heap_get_free_blocks(heap);

	if (size < sizeof(struct free_block)) {
		size = sizeof(struct free_block);
	}

	size = (size + (3)) & ~(3); /* align by word*/

	for (link = free_blocks_list->next; link != free_blocks_list; link = link->next) {
		block = (struct free_block *) ((uintptr_t *) link - 1);
		if ((size + sizeof(block->size)) > get_clear_size(block->size)) {
			continue;
		}

		/* Change block to aligned subblock to allocate memory start from aligned address. */
		if (boundary != 0) {
			struct free_block *aligned_block;

			aligned_block = block_align(heap, block, boundary, size);

			if (NULL == aligned_block) {
				continue;
			}
			block = aligned_block;
		}

		/* To cut piece of memory from free block we need set three service information:
		 * 1. Two for new free block (in begin and end of block): sizeof(block->size) + sizeof(struct free_block).
		 * 2. One for new busy block (in begin of block): sizeof(block->size). */
		if ((size + 2 * sizeof(block->size))
				< (get_clear_size(block->size) - sizeof(struct free_block))) {
			block = cut(heap, block, size);
		} else {
			block_unlink(block);
			mark_block(block);
			mark_next(block);
			/* we already have size */
		}

		ret_addr = (void *) ((uintptr_t *) block + 1);

		heap_desc->count++;

		sched_unlock();
		return ret_addr;
	}
	sched_unlock();
	return NULL;
}

void bm_free(void *heap, void *ptr) {
	struct free_block *block;
	struct heap_desc *heap_desc = (struct heap_desc *) heap;

	assert(ptr);

	sched_lock();
	block = (struct free_block *) ((uintptr_t *) ptr - 1);

	if (!block_is_busy(block)) {
		sched_unlock();
		printk("***** free(): the block not busy\n");
		return; /* if we try to free block more than once */
	}

	assert(block_is_busy(block));

	heap_desc->count--;

	afterfree(ptr, (get_clear_size(block->size) - sizeof(block->size)));

	/* Free block */
	block_link(heap, block);
	set_end_size(block);
	clear_block(block);
	clear_next(block);

	/* And than concatenate with neighbors */
	block = concatenate_prev(block);
	block = concatenate_next(block);

	sched_unlock();
}

void bm_init(void *heap, size_t size) {
	struct heap_desc *heap_desc;
	struct free_block *block;
	struct free_block_link *free_blocks_list;
	size_t actual_free_mem;

	heap_desc = (struct heap_desc *) heap;
	heap_desc->count = 0;

	free_blocks_list = heap_get_free_blocks(heap);
	free_blocks_list->next = free_blocks_list;
	free_blocks_list->prev = free_blocks_list;

	/* initial block */
	block = heap + sizeof *heap_desc;
	/* structure of heap (@c size bytes):
	 * count | free_blocks_list | *** initial block *** | (sizeof block->size) bytes for busy block | */
	actual_free_mem = size - (sizeof *heap_desc + sizeof block->size);

	block->size = actual_free_mem;
	set_end_size(block);

	mark_prev(block);
	block_link(heap, block);

	/* last work we mark as persistence busy */
	block = (void *) block + get_clear_size(block->size);
	mark_block(block);
}

int bm_heap_is_empty(void *heap) {
	struct heap_desc *heap_desc = (struct heap_desc *) heap;
	return heap_desc->count == 0;
}
