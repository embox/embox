/**
 * @file
 * @brief Implementation of heap based on Boundary Markers
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
#include <util/math.h>
#include <util/binalign.h>
#include <kernel/printk.h>

#define DEBUG 0
#if DEBUG
#define printd(...) printk(__VA_ARGS__)
#else
#define printd(...) do {} while(0);
#endif

EMBOX_UNIT_INIT(heap_init);

#define HEAP_SIZE OPTION_MODULE_GET(embox__mem__heap_api,NUMBER,heap_size)

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

static void *pool;
static void *pool_end;
static struct free_block_link free_blocks = { &free_blocks, &free_blocks };

#define get_clear_size(size) ((size) & ~3)
#define get_flags(size) ((size) & 3)

static int block_is_busy(struct free_block *block) {
	return block->size & 0x1;
}

static int prev_is_busy(struct free_block *block) {
	return block->size & 0x2;
}

static void block_link(struct free_block *block) {
	block->link.next = free_blocks.next;
	block->link.prev = &free_blocks;
	free_blocks.next->prev = &block->link;
	free_blocks.next = &block->link;
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

	*(uint32_t *) ((void *) block + size - 4) = size;
}

static struct free_block * concatenate_prev(struct free_block *block) {
	size_t prev_size, new_size;
	struct free_block *pblock; /* prev block */

	if (prev_is_busy(block)) {
		return block;
	}

	prev_size = *((uint32_t *) ((uint32_t *) block - 1));
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
	uint32_t flags = get_flags(block->size);
	block->size = flags | size;
}

static struct free_block * cut(struct free_block *block, size_t size) {
	struct free_block *nblock; /* new block */
	size_t offset;

	offset = size + sizeof(block->size);
	nblock = (struct free_block *) ((char *) block + offset);

	block_unlink(block);
	block_link(nblock);

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
static struct free_block *block_align(struct free_block *block, size_t boundary, size_t size) {
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
	block_link(aligned_block);

	/* Init remainder */
	block_set_size(block, (size_t) aligned_block - (size_t) block);
	set_end_size(block);

	return aligned_block;
}

void *memalign(size_t boundary, size_t size) {
	struct free_block *block;
	struct free_block_link *link;
	void *ret_addr;

	if (size <= 0) {
		return NULL;
	}

	if (size < sizeof(struct free_block)) {
		size = sizeof(struct free_block);
	}

	size = (size + (3)) & ~(3); /* align by word*/

	for (link = free_blocks.next; link != &free_blocks; link = link->next) {
		block = (struct free_block *) ((uint32_t *) link - 1);
		if ((size + sizeof(block->size)) > get_clear_size(block->size)) {
			continue;
		}

		/* Change block to aligned subblock to allocate memory start from aligned address. */
		if (boundary != 0) {
			struct free_block *aligned_block;

			aligned_block = block_align(block, boundary, size);

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
			block = cut(block, size);
		} else {
			block_unlink(block);
			mark_block(block);
			mark_next(block);
			/* we already have size */
		}

		ret_addr = (void *) ((uint32_t *) block + 1);

		return ret_addr;
	}
	return NULL;
}

void * malloc(size_t size) {
	void *ptr;

	if (size == 0) {
		return NULL;
	}

	ptr = memalign(0, size);
	if (ptr == NULL) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	return ptr;
 }

void free(void *ptr) {
	struct free_block *block;

	if (ptr == NULL) {
		return;
	}

	if (ptr < pool || ptr >= pool_end) {
		printk("***** free(): incorrect address space\n");
		return;
	}
	/* assert((ptr < pool) || (ptr >= pool_end)); */;

	block = (struct free_block *) ((uint32_t *) ptr - 1);

	if (!block_is_busy(block)) {
		printk("***** free(): the block not busy\n");
		return; /* if we try to free block more than once */
	}

	assert(block_is_busy(block));

	/* Free block */
	block_link(block);
	set_end_size(block);
	clear_block(block);
	clear_next(block);

	/* And than concatenate with neighbors */
	block = concatenate_prev(block);
	block = concatenate_next(block);
}

void * realloc(void *ptr, size_t size) {
	struct free_block *block;
	void *ret;

	if (size == 0) {
		free(ptr);
		return NULL; /* ok */
	}

	ret = malloc(size);
	if (ret == NULL) {
		return NULL; /* error: errno set in malloc */
	}

	if (ptr == NULL) {
		printd("addr = 0x%X\n", (uint32_t)ret);
		return ret;
	}

	block = (struct free_block *) ((uint32_t *) ptr - 1);

	/* Copy minimum of @c size and actual size of object pointed by @c ptr. And then free @c ptr */
	memcpy(ret, ptr, min(size, get_clear_size(block->size) - sizeof(block->size)));
	free(ptr);

	printd("addr = 0x%X\n", (uint32_t)tmp);
	return ret;
}

void * calloc(size_t nmemb, size_t size) {
	void *ret;
	size_t total_size;

	total_size = nmemb * size;
	if (total_size == 0) {
		return NULL; /* ok */
	}

	ret = malloc(total_size);
	if (ret == NULL) {
		return NULL; /* error: errno set in malloc */
	}

	memset(ret, 0, total_size);
	return ret;
}

static int heap_init(void) {
	extern struct page_allocator *__heap_pgallocator;
	struct free_block *block;

	pool = page_alloc(__heap_pgallocator, HEAP_SIZE / PAGE_SIZE() - 2);
	if(NULL == pool) {
		return -1;
	}

	block = (struct free_block *) pool;
	block->size = HEAP_SIZE - (PAGE_SIZE() * 2) - sizeof(block->size);
	set_end_size(block);

	mark_prev(block);
	block_link(block);

	/* last work we mark as persistence busy */
	block = (void *) ((char *) pool + get_clear_size(block->size));
	mark_block(block);

	pool_end = block;

	return 0;
}
