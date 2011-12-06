/**
 * @file
 * @brief	Test memory allocation
 *
 * @date	5.12.11
 * @author	Gleb Efimov
 *
 */
#include <stdlib.h>
#include <types.h>
#include <framework/example/self.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

#define MEM_SIZE 0x1000000
#define NUMBER_OF_TESTS 100

#define CHECK(cfs, sz) \
	(cfs + sizeof(struct memory_descriptor) + sz) \
	< (memory + sizeof(memory))

struct memory_descriptor {
	char available;
	size_t size;
};

char* current_free_space;
char memory[MEM_SIZE];

void* memory_allocate(int size);

static int run(int argc,char **argv){
	struct memory_descriptor *md;
	int i, temp;
	void *succes, *address;
	current_free_space = memory;
	md = (void *)memory;
	md->available = 1;
	md->size = sizeof(memory);
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		succes = memory_allocate(temp = rand()%10000);
		if (succes == NULL) {
			printf("Memory allocation error on the addition #%d size of block: %d",i,temp);
		}
	}
	address = (void *)memory;
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		memory_free(*address);
	}
	return 0;
}

void* memory_allocate(int size) {
	struct memory_descriptor *current_md = (void *)current_free_space;
	while ((current_md->available == 0) & (current_md->size <= size + sizeof(struct memory_descriptor))) {
		current_md += current_md->size;
		if (current_md > memory + sizeof(memory)) return NULL;
	}
	if (CHECK(current_free_space, size))
	{
		current_md->available = 0;
		current_md->size = size + sizeof(struct memory_descriptor);
		current_free_space += current_md->size;
		return ((void *)(current_md + sizeof(struct memory_descriptor)));
	}
	return NULL;
}

void memory_free(void *address) {
	struct memory_descriptor *md = address - sizeof(struct memory_descriptor);
	md->available = 1;
	current_free_space = md;
}
