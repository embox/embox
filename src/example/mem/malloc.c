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

struct memory_descriptor {
	char available;
	size_t size;
};

#define MEM_DESC_SZ sizeof(struct memory_descriptor)

static char *current_free_space;
static char  memory[MEM_SIZE];

static void *memory_allocate(int size) {
	struct memory_descriptor *current_md = (void *) current_free_space;
	while (current_md->available == 0 &&
		current_md->size <= size + MEM_DESC_SZ) {
		current_md += current_md->size;
		if ((void *) current_md > (void *) (memory + sizeof(memory)))
			return NULL;
	}
	if (current_free_space + MEM_DESC_SZ + size < memory + sizeof(memory)) {
		int all_block = current_md->size + MEM_DESC_SZ;
		struct memory_descriptor *temp;
		current_md->available = 0;
		current_md->size = size + MEM_DESC_SZ;
		current_free_space += current_md->size;
		temp = (void *) current_free_space;
		temp->available = 1;
		temp->size = all_block - current_md->size;
		return (void *) (current_md + MEM_DESC_SZ);
	}
	return NULL;
}

static void memory_free(void *address) {
	struct memory_descriptor *md = address - MEM_DESC_SZ;
	md->available = 1;
	current_free_space = (char *) md;
}

static void memory_init(void) {
	struct memory_descriptor *md;
	md = (void *) memory;
	md->available = 1;
	md->size = sizeof(memory);
}

static int run(int argc, char **argv) {
	struct memory_descriptor *md;
	int i, temp;
	void *succes, *address;
	current_free_space = memory;
	memory_init();
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		succes = memory_allocate(temp = rand() % 10000);
		if (succes == NULL) {
			printf("Memory allocation error on the addition %d size of block: %d\n", i, temp);
		}
	}
	address = memory;
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		if (rand() % 2 == 0)
			memory_free(address);
		md = address - MEM_DESC_SZ;
		address += md->size;
	}
	return 0;
}
