/**
 * @file
 * @brief Example of simple memory allocation
 *
 * @date 05.12.11
 * @author Gleb Efimov
 * @author Daria Dzendzik
 */

#include <stdio.h>
#include <stdlib.h>
#include <types.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(run);
//TODO config
#define MEM_SIZE                     0x10000//00
#define QUANTITY_OF_TESTS            5

/* This structure describes memory block:
 * @available - state flag: available or not
 * the memory block for allocate (default - available) (0|1);
 * @size      - size of memory block (default - all dedicated space); */
struct block_desc {
	unsigned int is_available;
	size_t size;
};

/* Size of block_desc. */
#define BLOCK_DESC_SIZE  sizeof(struct block_desc)
/* The and of memory pool*/
#define LAST_ADDRESS     (memory + (uint32_t)sizeof(memory))

/* This is a pointer to the block
 * at current time in our memory.
 * We start to search from this place*/
static char *current_space;

/* This is our pool memory, that we allocated,
 * size defined from MEM_SIZE. */
static char  memory[MEM_SIZE];

/*This function check address correction
 * address must be from our memory */
static int correct_address(char *address){
	return (((uint32_t)address >= (uint32_t)memory) &&
			((uint32_t)address < (uint32_t)LAST_ADDRESS));
}

/* This function get field "available"
 * 1 - free, can alloc
 * 0 - not free  */
static int get_available(struct block_desc *md){
	return (md->is_available);
}

/*This function set field "available" available */
static void set_available(struct block_desc *block){
	block->is_available = 1;
}

/*This function set field "available" not available */
static void set_not_available(struct block_desc *block){
	block->is_available = 0;
}

/*This function check search block size*/
static int correct_size(struct block_desc *md, size_t req_size){
	return ((uint32_t)md->size >= (uint32_t)req_size + (uint32_t)BLOCK_DESC_SIZE);
}

/*This function search available block with correct size*/
struct block_desc *find_suit_block(size_t req_size) {
	/* Set the pointer(iterator) on the begin of our memory */
	struct block_desc *md = (void *) current_space;

	printf("looking for: %d\n", req_size);
	printf("current_free_space 0x%x\n", (uint32_t)current_space);


	if (correct_address(current_space) == 0){
		printf("address is not correct!!!  \n");
		return NULL;
	}
	/* While current block not available or req_size of block
	 * less then necessary req_size go to the next block.
	 * If the pointer(iterator) went for memory limits
	 * then return NULL */
	while ((!get_available(md)) || (!correct_size( md, req_size))) {
		md = (void *)(((size_t)md) + md->size);

		printf("md = 0x%X\n", (uint32_t)md);
		printf("block is not able for this new block \n");

		if (((uint32_t)md) >= (uint32_t)(memory) + sizeof(memory)){
			/*end of memory*/
			return NULL;
		}
	}

	printf("need %d\n", (uint32_t)(req_size) + (uint32_t)(BLOCK_DESC_SIZE));
	printf("find 0x%x\n", (uint32_t)md);
	//printf("current_free_space 0x%x\n", (uint32_t)current_free_space);

	return md;
}

/* This is a function, which allocate block with necessary @size.
 * @size - size of needed block;
 * return - pointer to the allocated memory; */
static void *memory_allocate(size_t req_size) {
	/* allocated part */
	struct block_desc *new_block;
	/* another part of existing block */
	struct block_desc *old_block;

	if(NULL == (new_block = find_suit_block(req_size))) {
		return NULL;
	}

	/* Change state flag on unavailable*/
	set_not_available(new_block);

	if((new_block->size - req_size - BLOCK_DESC_SIZE) >= BLOCK_DESC_SIZE){
		/* Fixed req_size of block */
		new_block->size = req_size +  BLOCK_DESC_SIZE;

		/* Initializes a new block on the remaining part of block */
		old_block = (void *) ((size_t)new_block + req_size + BLOCK_DESC_SIZE);
		set_available(old_block);
		old_block->size = new_block->size - req_size - BLOCK_DESC_SIZE;
	}

	if(current_space == (void*) new_block) {
		current_space += (uint32_t)(new_block->size); // check types
	}

	printf("returned 0x%x\n", (uint32_t)(new_block) + (uint32_t)(BLOCK_DESC_SIZE));
	printf("current_free_space 0x%x\n", (uint32_t)current_space);

	return (void *) ((uint32_t)(new_block) + (uint32_t)(BLOCK_DESC_SIZE));
}

static void resolve_dafrag(struct block_desc *md){
	struct block_desc *next_md;
	next_md = (void *)(((size_t)md) + md->size);
	if (get_available(next_md)){
		md->size = md->size + next_md->size;
	}
}


/* This procedure makes free busy block
 * at the specified @address.
 * @address - block address being freed; */
static void memory_free(void *address) {

	/* Detect address of memory_block */
	struct block_desc *md = address - BLOCK_DESC_SIZE;
	/* Make block free*/
	md->is_available = 1;

	/* Set the new value of pointer to the free block */
	if (current_space > (char *) md) {
		printf("***\n");
		current_space = (char *) md;
	}

	/*Resolve defragmentation*/
	resolve_dafrag(md);

	printf("NEW current_free_space 0x%x\n", (uint32_t)current_space);

}

/* This is procedure of the beginning
 * initialization memory. */
static void memory_init(void) {
	struct block_desc *md;

	current_space = memory;

	printf("start of memory = 0x%X\n", (uint32_t)memory);
	printf("end of memory = 0x%X\n", ((uint32_t)memory) + (uint32_t)sizeof(memory));
	//printf("LAST_ADDRESS = 0x%X\n", (uint32_t)LAST_ADDRESS);
	//printf("BLOCK_DESC_SIZE = 0x%X\n", (uint32_t)BLOCK_DESC_SIZE);

	md = (void *) memory;
	md->is_available = 1;
	md->size = sizeof(memory);
}

/* This program tests the simplest algorithm of memory allocation */
static int run(int argc, char **argv) {
	int i, temp;
	void *succes_alloc[QUANTITY_OF_TESTS];

	memory_init();

	for (i = 0; i < QUANTITY_OF_TESTS; i++) {
		printf("\n\n");
		printf("==NEW BLOCK==\n");
		succes_alloc[i] = memory_allocate(temp = rand() % 1000);
		if (succes_alloc[i] == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
		} else {
			printf("block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
		}
	}

	i = 1;//for (i = 0; i < QUANTITY_OF_TESTS; i++) {
		if (rand() % 2 == 0){
			memory_free(succes_alloc[i]);
			printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
	}

	for (i = 0; i < QUANTITY_OF_TESTS; i++) {
		printf("\n\n");
		printf("==NEW BLOCK==\n");
		succes_alloc[i] = memory_allocate(temp = rand() % 10000);
		if (succes_alloc[i] == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
		}
		printf("alloc again block num = %d address = 0x%X\n", i, (uint32_t)succes_alloc[i]);
	}

	return 0;
	}
