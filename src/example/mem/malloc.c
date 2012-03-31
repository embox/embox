/**
 * @file
 * @brief Example of simple memory allocation
 *
 *
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
#if 0
	printf("current_free_space 0x%x\n", (uint32_t)current_space);
#endif

	/*Check address correction*/
	if (correct_address(current_space) == 0){
		printf("address 0x%X is not correct!!!  \n", (uint32_t)current_space);
		return NULL;
	}
	/* While current block not available or req_size of block
	 * less then necessary req_size go to the next block.
	 * If the pointer(iterator) went for memory limits
	 * then return NULL */
	while ((!get_available(md)) || (!correct_size( md, req_size))) {
		md = (void *)(((size_t)md) + md->size);
#if 0
		printf("md = 0x%X\n", (uint32_t)md);
		printf("block is not able for this new block \n");
#endif
		/*Check end of memory*/
		if (((uint32_t)md) >= (uint32_t)(memory) + sizeof(memory)){
			return NULL;
		}
	}

	//printf("need %d\n", (uint32_t)(req_size) + (uint32_t)(BLOCK_DESC_SIZE));
	//printf("find 0x%x\n", (uint32_t)md);

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

	/*If suit block is not exist, allocation is impossible*/
	if(NULL == (new_block = find_suit_block(req_size))) {
		return NULL;
	}

	/* Change state flag on unavailable*/
	set_not_available(new_block);
	/*If  */
	if((new_block->size - req_size - BLOCK_DESC_SIZE) >= BLOCK_DESC_SIZE){
		/* Initializes a new block on the remaining part of block */
		old_block = (void *) ((size_t)new_block + req_size + BLOCK_DESC_SIZE);
		set_available(old_block);
		old_block->size = new_block->size - req_size - BLOCK_DESC_SIZE;
		/* Fixed req_size of block */
		new_block->size = req_size +  BLOCK_DESC_SIZE;
	}

	/*If select min free space, change enter to search */
	if(current_space == (void*) new_block) {
		current_space += (uint32_t)(new_block->size); /* check types*/
	}

	printf("returned 0x%x\n", (uint32_t)(new_block) + (uint32_t)(BLOCK_DESC_SIZE));
	printf("current_free_space 0x%x\n", (uint32_t)current_space);

	/*Return point to memory in block*/
	return (void *) ((uint32_t)(new_block) + (uint32_t)(BLOCK_DESC_SIZE));
}

/*Resolve defragmentation with next block*/
static void resolve_defrag(struct block_desc *md){
	struct block_desc *next_md;

	next_md = (void *)(((size_t)md) + md->size);

	if(!correct_address((char *) md)){
		return;
	}

	if(get_available(md)){
		/*Look at the next block and it is free, paste it*/
		if (get_available(next_md)){
			md->size = md->size + next_md->size;
			resolve_defrag(md);
		}
	}
	resolve_defrag(next_md);
}


/* This procedure makes free busy block
 * at the specified @address.
 * @address - block address being freed; */
static void memory_free(void *address) {

	/* Detect address of memory_block */
	struct block_desc *md = address - BLOCK_DESC_SIZE;
	/* Make block free*/
	set_available(md);

	/* Set the new value of pointer to the free block */
	if (current_space > (char *) md) {
		current_space = (char *) md;
	} else {
		md = (void *) current_space;
	}

	/*Resolve defragmentation*/
	resolve_defrag(md);

	printf("NEW current_free_space 0x%x\n", (uint32_t)current_space);
}

/* This is procedure of the beginning
 * initialization memory. */
static void memory_init(void) {
	struct block_desc *md;

	/*Set search pointer to memory start */
	current_space = memory;

	printf("start of memory = 0x%X\n", (uint32_t)memory);
	printf("end of memory = 0x%X\n", ((uint32_t)memory) + (uint32_t)sizeof(memory));
#if 0
	printf("LAST_ADDRESS = 0x%X\n", (uint32_t)LAST_ADDRESS);
	printf("BLOCK_DESC_SIZE = 0x%X\n", (uint32_t)BLOCK_DESC_SIZE);
#endif
	/*Initialization first block */
	md = (void *) memory;
	md->is_available = 1;
	md->size = sizeof(memory);
}

/* This is synthetic algorithm of memory allocation tests with different situation */

static void input_mem(void){
	int p = 0;
	struct block_desc *md;
	md = (void *) memory;

	while (correct_address((char *) md)){
		printf("block num = %d\n ", p);
		printf("   address = 0x%X\n", (uint32_t)md);
		printf("   available = %d\n", get_available(md));
		printf("   size = 0x%X\n", (uint32_t)md->size);
		md = (void *)(((size_t)md) + md->size);
		p++;
	}
}

/**/
static void example(void){
	int i, j;
	/* Just a synthetic example*/
	/*Allocation 18 times*/
	int number_add_block = 18, I_alloc = 9, II_alloc = 8;
	uint32_t k = 0x100;																					//!!!!!!
	void *succes_alloc[number_add_block];
	int koef_alloc[] = {/*First allocation:  I_alloc = 9*/   1, 2, 3, 1, 4, 8, 3, 2, 5,
						/*Second allocation: II_alloc = 8*/  7, 4, 1, 2, 1, 1, 6, 267,
						/*Third allocation:  III_alloc = 1*/ 16};
	/*Free 12 block*/
	int I_free = 5, II_free = 7;
	int number_of_block[] = {/*First free:  I_free = 5*/  3, 6, 2, 4, 8,
							 /*Second free: II_free = 7*/  12, 1, 7, 14, 15, 5, 10};

	/*First allocation*/
	for (i = 0; i < I_alloc; i++) {
		printf("==NEW BLOCK==\n");
		succes_alloc[j] = memory_allocate(koef_alloc[j]*k-8);
		if (succes_alloc[j] == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", j, koef_alloc[j]*k);
		} else {
			printf("block num = %d address = 0x%X\n\n", j, (uint32_t)succes_alloc[j]);
		}
		j++;
	}

	/*First memory free*/
	for (i = 0; i < I_free; i++) {
		memory_free(succes_alloc[number_of_block[i]-1]);
		printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
	}

	/*Second allocation*/
	for (i = 0; i < II_alloc; i++) {
		printf("==NEW BLOCK==\n");
		if(j == 13){
			succes_alloc[j] = memory_allocate(koef_alloc[j]*k-8-8);
		} else {
			if (j == 14){
				succes_alloc[j] = memory_allocate(koef_alloc[j]*k-8-3);
			} else {
				succes_alloc[j] = memory_allocate(koef_alloc[j]*k-8);
			}
		}
		if (succes_alloc[j] == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", j, koef_alloc[j]*k);
		} else {
			printf("block num = %d address = 0x%X\n\n", j, (uint32_t)succes_alloc[j]);
		}
		j++;
	}

	/*Second memory free*/
	for (i = I_free; i < I_free + II_free; i++) {
		memory_free(succes_alloc[number_of_block[i]-1]);
		printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
	}

	/*Third allocation*/
	printf("==NEW BLOCK==\n");
	succes_alloc[j] = memory_allocate(koef_alloc[j]*k-8);
	if (succes_alloc[j] == NULL) {
		printf("\nMemory allocation error on the addition %d size of block: %d\n", j, koef_alloc[j]*k);
	} else {
		printf("block num = %d address = 0x%X\n\n", j, (uint32_t)succes_alloc[j]);
	}
	printf("----------------------------------------- \n");
	/*Check a result*/
	input_mem();
}


static int run(int argc, char **argv) {
#if 0
	int i, temp;
	void *succes_alloc[QUANTITY_OF_TESTS];
#endif

	memory_init();
	example();
#if 0
	/*First allocation*/
	for (i = 0; i < QUANTITY_OF_TESTS; i++) {
		printf("\n\n");
		printf("==NEW BLOCK==\n");
		succes_alloc[i] = memory_allocate(temp = rand() % 10000);
		if (succes_alloc[i] == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
		} else {
			printf("block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
		}
	}

	/*Memory free*/
#if 0
	for (i = 0; i < QUANTITY_OF_TESTS; i++) {
		if (rand() % 2 == 0){
			memory_free(succes_alloc[i]);
			printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);

		}
	}
#endif
//#if 0
	i=3;
	memory_free(succes_alloc[i]);
	printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
	i=1;
	memory_free(succes_alloc[i]);
	printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
	i=2;
	memory_free(succes_alloc[i]);
	printf("free block num = %d address = 0x%X\n\n", i, (uint32_t)succes_alloc[i]);
//#endif

	/*Allocation after free*/
	for (i = 0; i < QUANTITY_OF_TESTS; i++) {
		printf("\n\n");
		printf("==NEW BLOCK==\n");
		succes_alloc[i] = memory_allocate(temp = rand() % 10000);
		if (succes_alloc[i] == NULL) {
			printf("\nMemory allocation error on the addition %d size of block: %d\n", i, temp);
		}
		printf("alloc again block num = %d address = 0x%X\n", i, (uint32_t)succes_alloc[i]);
	}
#endif
	return 0;
	}
