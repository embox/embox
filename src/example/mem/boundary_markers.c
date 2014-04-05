/**
 * @file
 * @brief
 *
 * @date 18.11.2011
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <framework/example/self.h>
#include <mem/page.h>


EMBOX_EXAMPLE(run);

struct example_bm_free_block;

struct example_bm_free_block_link {
	struct example_bm_free_block_link *prev;
	struct example_bm_free_block_link *next;
};

/* sizeof() = 16 */
struct example_bm_free_block {
	size_t size;

	struct example_bm_free_block_link link;

	union {
		uint32_t *tail;
		size_t end_size;
	} end;
};

static struct example_bm_free_block_link free_blocks = {
	&free_blocks,
	&free_blocks
};

static void free_block_add(void *block, size_t size) {
	struct example_bm_free_block *header = block;

	header->size = size;

	header->link.prev = &free_blocks;
	header->link.next = free_blocks.next;
	free_blocks.next->prev = &header->link;
	free_blocks.next = &header->link;

	/* set size and marker to the and of the block */
	header->end.tail[size - 2] = size;
}

static void example_heap_init(void *pool, size_t size) {
	free_block_add(pool, size);
}

static inline void mark_busy(struct example_bm_free_block *block) {
	block->end.tail[block->size] = block->size | 1;
	block->size |= 0x1;
}

static void *example_malloc(size_t size) {
	struct example_bm_free_block *block;
	struct example_bm_free_block_link *link;

	if(size <= 0) {
		return NULL;
	}

	for (link = free_blocks.next; link != &free_blocks; link = link->next) {
		block = (struct example_bm_free_block *)(((int)link)- 4);
		if (size > block->size) {
			continue;
		}

		if (size >= block->size && size <= (block->size + sizeof(*block))) {
			/*list_unlink();*/
			link->next->prev = link->prev;
			link->prev->next = link->next;

			mark_busy(block);
			return (uint32_t *) block + 1;
		}
		if (size < block->size) { /* cut a peace of the block*/
			/* */
			struct example_bm_free_block *new_block;

			new_block = (struct example_bm_free_block *)((char *) block + size);

			new_block->link.next = block->link.next;
			new_block->link.prev = block->link.prev;
			new_block->size = block->size - size;
			new_block->end.tail[new_block->size - 2] = new_block->size;

			block->size = size;
			block->link.prev->next = &new_block->link;
			block->link.next->prev = &new_block->link;

			mark_busy(block);

			return (uint32_t *) block + 1;
		}
	}
	return NULL;
}

static void example_free(void *obj) {

}

static void obj_allocation(void) {
	void * obj1, *obj2;

	obj1 = example_malloc(20);
	obj2 = example_malloc(20);
	example_free(obj1);
	example_free(obj2);
}

static int run(int argc, char **argv) {
	void *pool;

	pool = page_alloc(10);

	example_heap_init(pool, PAGE_SIZE());

	obj_allocation();

	page_free(pool, 10);

	return 0;
}
