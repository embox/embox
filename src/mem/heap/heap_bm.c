/**
 * @file
 * @brief Implementation of heap based on Boundary Markers
 *
 * @date 24.11.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <mem/page.h>
#include <string.h>
#include <math.h>
#include <embox/unit.h>
#include <mem/heap.h>

#include <stdlib.h>

#define DEBUG 0
#if DEBUG
#include <kernel/printk.h>

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
	size_t size;
	struct free_block_link link;
	size_t end_size;
};

static void *pool;
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

static void clear_prev(struct free_block *block) {
	block->size &= ~0x2;
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
	new_size = get_clear_size(prev_size) + get_clear_size(block->size); /* block->size is clear) */
	pblock->size = new_size | 0x2; /* prev is busy */
	set_end_size(pblock);

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

	size += get_clear_size(nblock->size);
	block->size = size | get_flags(block->size);
	set_end_size(block);
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

	nblock->size = get_clear_size(block->size) - offset;
	set_end_size(nblock);

	block_set_size(block, offset);

	mark_prev(nblock);

	return block;
}

static void *block_aligning(struct free_block *block, size_t boundary) {
	struct free_block *aligned_block;
	void *ret_addr;
	size_t size;

	ret_addr =
			(void *) ((size_t) (block) + sizeof(struct free_block) + boundary);
	ret_addr = (void *) ((size_t) (ret_addr) & ~(boundary - 1));

	aligned_block =
			(struct free_block *) ((size_t) ret_addr - sizeof(ret_addr));

	size = get_clear_size(block->size) - ((size_t) aligned_block - (size_t) block);

	aligned_block->size = block->size; /* copy flags*/
	block_set_size(aligned_block, size);
	mark_block(aligned_block);
	clear_prev(aligned_block);

	ret_addr = (void *) ((uint32_t *) aligned_block + 1);

	block_set_size(block, (size_t) aligned_block - (size_t) block);
	block_link(block);
	clear_block(block);
	set_end_size(block);
	clear_next(block);

	return ret_addr;
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

	if (0 != boundary) {
		// boundary = max(boundary, sizeof(struct free_block) << 1);
		size = (size + 1 + (sizeof(struct free_block) << 1) +(boundary - 1)) & ~(boundary - 1);
	} else {
		size = (size + (3)) & ~(3); /* align by word*/
	}

	for (link = free_blocks.next; link != &free_blocks; link = link->next) {
		block = (struct free_block *) ((uint32_t *) link - 1);
		if ((size + sizeof(block->size)) > get_clear_size(block->size)) {
			continue;
		}

		if ((size + sizeof(block->size))
				< (get_clear_size(block->size) - sizeof(struct free_block))) {
			block = cut(block, size);
			mark_block(block);
			mark_next(block);
		} else {
			block_unlink(block);
			mark_block(block);
			mark_next(block);
			/* we already have size */
		}

		ret_addr = (void *) ((uint32_t *) block + 1);
		if (0 != ((size_t) ret_addr & ~(boundary - 1))) {
			ret_addr = block_aligning(block, boundary);
		}

		return ret_addr;
	}
	return NULL;
}

void *malloc(size_t size) {
	return memalign(0, size);
 }

#include <stdio.h>
/*#include <assert.h>*/
void free(void *ptr) {
	struct free_block *block;
	if (ptr == NULL) {
		return;
	}
	block = (struct free_block *) ((uint32_t *) ptr - 1);

	if (!block_is_busy(block)) {
		printf("DOUBLE FREE!!\n");
		/*assert(0)*/;
		return; /* if we try to free block more than once */
	}

	block = concatenate_prev(block);
	block = concatenate_next(block);

	if (block_is_busy(block)) {
		block_link(block);
		clear_block(block);
		clear_next(block);

		set_end_size(block);
	}
}

void *realloc(void *ptr, size_t size) {
	struct free_block *block;
	void *tmp;

	if (NULL == ptr) {
		tmp = malloc(size);
		printd("addr = 0x%X\n", (uint32_t)tmp);

		return tmp;
	}

	tmp = malloc(size);
	block = (struct free_block *) ((uint32_t *) ptr - 1);

	memcpy(tmp, ptr, min(size, get_clear_size(block->size)));
	printd("addr = 0x%X\n", (uint32_t)tmp);
	return tmp;
}

void *calloc(size_t nmemb, size_t size) {
	void *tmp = malloc(nmemb * size);
	if (tmp) {
		memset(tmp, 0, nmemb * size);
	}
	return tmp;
}

static int heap_init(void) {
	extern struct page_allocator *__heap_pgallocator;
	struct free_block *block;

	pool = page_alloc(__heap_pgallocator, HEAP_SIZE / PAGE_SIZE() - 2);
	if(NULL == pool) {
		return -1;
	}

	block = (struct free_block *) pool;
	block->size = HEAP_SIZE / 2 - sizeof(block->size);
	set_end_size(block);

	mark_prev(block);
	block_link(block);

	/* last work we mark as persistence busy */
	block = (void *) ((char *) pool + HEAP_SIZE - sizeof(block->size));
	mark_block(block);

	return 0;
}
