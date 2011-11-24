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



//static int block_is_busy(struct free_block *block) {
//	return block->size & 0x1;
//}

static int prev_is_busy(struct free_block *block) {
	return block->size & 0x2;
}

#define get_clear_size(size) (size & ~3)
#define get_flags(size) (size & 3)



static struct free_block_link free_blocks = {&free_blocks,  &free_blocks};

static struct free_block * concatenate_prev(struct free_block *block){
	size_t prev_size, new_size;
	struct free_block *nblock;

	if(prev_is_busy(block)) {
		return block;
	}
	prev_size = *((uint32_t *)((void *)block - 1));
	nblock = (struct free_block *)((char *)block + get_clear_size(prev_size));
	new_size = get_clear_size(prev_size) + block->size; /* block->size is clear) */
	nblock->size = new_size | 0x2; /* prev is busy */
	return nblock;
}

static struct free_block * concatenate_next(struct free_block *block) {
	return NULL;
}

static void cut(struct free_block *block, size_t size) {


}

void *malloc(size_t size) {
	struct free_block *block;
	struct free_block_link *link;

	if(size <= 0) {
		return NULL;
	}
	for(link = free_blocks.next; link != &free_blocks; link = link->next) {
		block = (struct free_block *)(((int)link)- 4);
		if(size < (block->size)) { /* cut a peace of the block*/
			cut(block, size);
		}
	}
	return NULL;

}

void free(void* ptr) {
	struct free_block *block = (struct free_block *)((uint32_t *)ptr - 1);

	block = concatenate_prev(block);

	concatenate_next(block);
}

static int heap_init(void) {
	page_alloc(CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE);

	return 0;
}
