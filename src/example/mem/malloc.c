/**
 * @file
 * @brief Example of simple memory allocation
 *
 * @date 05.12.11
 * @author Gleb Efimov
 */

#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(run);

#define MEM_SIZE        0x100000
#define NUMBER_OF_TESTS 100

/* This structure described memory block:
 * @available - state flag: available or not
 * the memory block for allocate (default - available) (0|1);
 * @size      - size of memory block (default - all dedicated space); */
struct block_desc {
	char available;
	size_t size;
};

/* Size of block_desc. */
#define BLOCK_DESC_SIZE sizeof(struct block_desc)

/* This is a pointer to the free block
 * at current time in our memory. */
static char *current_free_space;

/* This is our memory, that we allocated,
 * size defined from MEM_SIZE. */
static char  memory[MEM_SIZE];
//static char *mem;

static char is_notavailable(struct block_desc *md){
	if (md->available == 0)
		return 1;
	else
		return 0;
}

static char disable_size(size_t req_size, struct block_desc *md){
	if (md->size <= req_size + BLOCK_DESC_SIZE)
		return 1;
	else
		return 0;
}

struct block_desc *find_suit_block(size_t req_size) {
	/* Set the pointer(iterator) on the begin of our memory */
	struct block_desc *md = (void *) current_free_space;
	printf("^");
	//mem = (&memory[MEM_SIZE])+1;
	/* While current block not available or req_size of block
	 * less then necessary req_size go to the next block.
	 * If the pointer(iterator) went for memory limits
	 * then return NULL */
	while (is_notavailable(md) && disable_size(req_size, md)) {
		md = (void *)((size_t *) md + md->size);   // должно быть приведение типа, потому что мы прибавляем size*размер структуры
		if ((void *) md >= (void *)(memory + sizeof(memory)*MEM_SIZE) ){ //
			printf("@");
			return NULL;
		}
	}
	printf(" $ ");
	return md;
}

/* This is a function, which allocate block with necessary @size.
 * @size - size of needed block;
 * return - pointer to the allocated memory; */
static void *memory_allocate(size_t req_size) {

	struct block_desc *new_block;
	struct block_desc *old_block;

	if(NULL == (new_block = find_suit_block(req_size))) {
		return NULL;
	}
	printf("");
	/* Set the pointer of current free block
	 * to the memory for the rest of the old block */
	current_free_space += new_block->size;

	/* Initializes a new block on the remaining part of block */
	old_block = (void *) current_free_space;
	old_block->available = 1;
	old_block->size = new_block->size - req_size;

	/* Change state flag on unavailable
	 * and fixed req_size of block */
	new_block->available = 0;
	new_block->size = req_size + BLOCK_DESC_SIZE;
	printf("*");
	return (void *) (new_block + BLOCK_DESC_SIZE);
}

/* This procedure makes free busy block
 * at the specified @address.
 * @address - block address being freed; */
static void memory_free(void *address) {

	/* Detect address of memory_block */
	struct block_desc *md = address - BLOCK_DESC_SIZE;

	/* Make block free*/
	md->available = 1;

	/* Set the new value of pointer to the free block */
	if (current_free_space > (char *)md) {
		current_free_space = (char *) md;
	}
}

/* This is procedure of the beginning
 * initialization memory. */
static void memory_init(void) {
	struct block_desc *md;

	current_free_space = memory;
	md = (void *) memory;
	md->available = 1;
	md->size = sizeof(memory);
}

/* This program tests the simplest algorithm of memory allocation */
static int run(int argc, char **argv) {
	struct block_desc *md;
	int i, temp;
	void *succes_alloc, *address;
	printf("1");
	memory_init();
	printf("2");
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		succes_alloc = memory_allocate(temp = rand() % 10000);
		if (succes_alloc == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
		}
	}
	printf("3");
//FIXME WTF?
	address = memory;
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		if (rand() % 2 == 0)
			memory_free(address);
		md = address - BLOCK_DESC_SIZE;
		address += md->size;
	}
	printf("4");

	for (i = 0; i < NUMBER_OF_TESTS; i++) {
			printf("&");
			succes_alloc = memory_allocate(temp = rand() % 1000);
			if (succes_alloc == NULL) {
				printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
			}
		}
	printf("5");
	return 0;
}
