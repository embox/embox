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
 * @size      - size of memory block (default - all dedicated space);
 */
struct memory_descriptor {
	char available;
	size_t size;
};

/* Size of memory_descriptor.
 */
#define MEM_DESC_SZ sizeof(struct memory_descriptor)

/* This is a pointer to the free block
 * at current time in our memory.
 */
static char *current_free_space;

/* This is our memory, that we allocated,
 * size defined from MEM_SIZE.
 */
static char  memory[MEM_SIZE];

/* This is a function, which allocate block with necessary @size.
 * @size - size of needed block;
 * return - pointer to the allocated memory;
 */
static void *memory_allocate(int size) {
	/* Set the pointer(iterator) on the begin of our memory */
	struct memory_descriptor *current_md = (void *) current_free_space;

	/* While current block not available or size of block
	 * less then necessary size go to the next block.
	 * If the pointer(iterator) went for memory limits
	 * then return NULL */
	while (current_md->available == 0 &&
		current_md->size <= size + MEM_DESC_SZ) {
		current_md += current_md->size;
		if ((void *) current_md > (void *) (memory + sizeof(memory)))
			return NULL;
	}

	/* Operational data */
	int all_block = current_md->size + MEM_DESC_SZ;
	struct memory_descriptor *temp;

	/* Change state flag on unavailable
	 * and fixed size of block */
	current_md->available = 0;
	current_md->size = size + MEM_DESC_SZ;

	/* Set the pointer of current free block
	 * to the memory for the rest of the old block */
	current_free_space += current_md->size;

	/* Initializes a new block on the remaining part of block */
	temp = (void *) current_free_space;
	temp->available = 1;
	temp->size = all_block - current_md->size;

	return (void *) (current_md + MEM_DESC_SZ);
}

/* This procedure makes free busy block
 * at the specified @address.
 * @address - block address being freed;
 */
static void memory_free(void *address) {
	/* Detect address of memory_block */
	struct memory_descriptor *md = address - MEM_DESC_SZ;
	/* Make block free*/
	md->available = 1;
	/* Set the new value of pointer to the free block */
	if (current_free_space > (char *)md) current_free_space = (char *) md;
	return;
}

/* This is procedure of the beginning
 * initialization memory.
 */
static void memory_init(void) {
	struct memory_descriptor *md;
	current_free_space = memory;
	md = (void *) memory;
	md->available = 1;
	md->size = sizeof(memory);
	return;
}

/* This program tests the simplest algorithm of memory allocation
 */
static int run(int argc, char **argv) {
	struct memory_descriptor *md;
	int i, temp;
	void *succes_alloc, *address;
	memory_init();
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		succes_alloc = memory_allocate(temp = rand() % 10000);
		if (succes_alloc == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
		}
	}
	address = memory;
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		if (rand() % 2 == 0)
			memory_free(address);
		md = address - MEM_DESC_SZ;
		address += md->size;
	}
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
			succes_alloc = memory_allocate(temp = rand() % 1000);
			if (succes_alloc == NULL) {
				printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
			}
		}
	return 0;
}
