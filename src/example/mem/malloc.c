/**
 * @file
 * @brief	Test memory allocation
 *
 * @date	5.12.11
 * @author	Gleb Efimov
 *
 */
#include <stdlib.h>
#include <framework/example/self.h>

/**
 * This macro is used for registration of this example at system
 * run - function which describe work of example
 */
EMBOX_EXAMPLE(run);

#define MEM_SIZE 0x1000000
#define NUMBER_OF_TESTS 100

#define CHECK(cfs, md, sz, memory) \
	(cfs + sizeof(md) + sz) \
	< (memory + sizeof(memory))

struct memory_descriptor {
	char available;
	size_t size;
};

char* current_free_space;
char memory[MEM_SIZE];

char memory_allocate(int size);

static int run(int argc,char **argv){
	struct memory_descriptor *md;
	int i;
	char succes;
	current_free_space = memory;
	md = (void *)memory;
	md->available = 1;
	md->size = sizeof(memory);
	for (i = 0; i < NUMBER_OF_TESTS; i++) {
		succes = memory_allocate(rand());
		if (!succes) break;
	}
	return 0;
}

char memory_allocate(int size) {
	if (CHECK(current_free_space,struct memory_descriptor, size, memory))
	{
		struct memory_descriptor *current_md = (void *)current_free_space;
		current_md->available = 0;
		current_md->size = size + sizeof(struct memory_descriptor);
		current_free_space += current_md->size;
		return (char)1;
	}
	return (char)0;
}
