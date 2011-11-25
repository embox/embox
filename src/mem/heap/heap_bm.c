/**
 * @file
 *
 * @brief
 *
 * @date 24.11.2011
 * @author Anton Bondarev
 */
#include <types.h>
#include <mem/page.h>
#include <string.h>
#include <embox/unit.h>

#include <stdlib.h>

EMBOX_UNIT_INIT(heap_init);

struct free_block_link {
	struct free_block_link *prev;
	struct free_block_link *next;
};

struct free_block {
	size_t size;
	struct free_block_link link;
	uint32_t *tail;
};

static void *pool;
static struct free_block_link free_blocks = {&free_blocks,  &free_blocks};


#define get_clear_size(size) (size & ~3)
#define get_flags(size) (size & 3)

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

static void mark_prev(struct free_block *block) {
	block->size |= 0x2;
}

static void mark_next(struct free_block *block) {
	struct free_block *nblock; /* next block */

	nblock = (struct free_block *)((char *)block + get_clear_size(block->size));

	nblock->size |= 0x2;
}


static struct free_block * concatenate_prev(struct free_block *block){
	size_t prev_size, new_size;
	struct free_block *pblock; /* prev block */

	if(prev_is_busy(block)) {
		return block;
	}

	prev_size = *((uint32_t *)((void *)block - 1));
	pblock = (struct free_block *)((char *)block - get_clear_size(prev_size));
	new_size = get_clear_size(prev_size) + block->size; /* block->size is clear) */
	pblock->size = new_size | 0x2; /* prev is busy */

	return pblock;
}

static struct free_block * concatenate_next(struct free_block *block) {
	size_t size;
	struct free_block *nblock; /* next block */

	nblock = (struct free_block *)((char *)block + get_clear_size(block->size));

	if(block_is_busy(nblock)) {
		return block;
	}

	size = get_clear_size(nblock->size) + get_clear_size(block->size);
	block->size = size | get_flags(block->size);

	block_unlink(nblock);

	return NULL;
}


static struct free_block * cut(struct free_block *block, size_t size) {
	struct free_block *nblock; /* new block */

	nblock = (struct free_block *)((char *)block + size + sizeof(block->size));

	block_unlink(block);
	block_link(nblock);

	mark_prev(nblock);

	return block;
}

void *malloc(size_t size) {
	struct free_block *block;
	struct free_block_link *link;

	if(size <= 0) {
		return NULL;
	}

	size = (size + 3) & ~3;

	for(link = free_blocks.next; link != &free_blocks; link = link->next) {
		block = (struct free_block *)(((void *)link) - 1);
		if(size <= get_clear_size(block->size) + sizeof(block->size)) {
			block_unlink(block);
			mark_block(block);
			mark_next(block);
			return (void *)((uint32_t *)(block) + 1);
		}
		if(size <= (get_clear_size(block->size) + sizeof(block->size) + sizeof(struct free_block))) {
			block = cut(block, size);
			mark_block(block);
			mark_next(block);
			return (void *)((uint32_t *)(block) + 1);
		}
	}
	return NULL;
}

void free(void* ptr) {
	struct free_block *block = (struct free_block *)((uint32_t *)(ptr) - 1);

	block = concatenate_prev(block);

	block = concatenate_next(block);

	if(block_is_busy(block)) {
		block_link(block);
	}
}

static int heap_init(void) {
	struct free_block *block;

	pool = page_alloc(CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE);

	block = (struct free_block *)pool;
	block->size = CONFIG_HEAP_SIZE;

	mark_prev(block);

	block_link(block);

	return 0;
}
