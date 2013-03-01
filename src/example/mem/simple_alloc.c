/**
 * @file
 *
 * @brief Example of simple memory allocation
 *
 * @details Simple memory allocation algorithm
 *
 * All memory divide on blocks. There is descriptor with "available" and size before every block.
 *
 * @par Initialization
 * All available space is one free block. current_space is start of memory.
 * The whole memory is divided into blocks. There is a descriptor with availability mark and size in front of each block. Initially the whole available space is represented by a single free block and #current_space points to the start of the memory.
 *
 * @par Allocation
 * During allocation we, first of all, search for the first free block with suitable size. If there is no such block, we give up. Otherwise, if we found a block and it is large enough (that is if a block that would remain after allocating the current one is bigger than the descriptor), then it is divided into two blocks - a newly allocated one and the rest free block.
 *
 * @par Freeing
 * Mark a block being freed as "free" and perform defragmentation.
 *
 * @date 05.12.11
 *
 * @author Gleb Efimov
 * @author Daria Dzendzik
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(run);

/* This structure describes memory block header. It contains two fields:
 * @available - is a state flag whether the block is now using or not
 * @size      - size of memory block (default - all dedicated space); */
struct block_desc {
	unsigned int is_available; /**< 1 is block is free. 0 means block is used*/
	size_t size; /** full size of the block including the block descriptor */
};

/* Helper macro. Size of a block_desc structure. */
#define BLOCK_DESC_SIZE  sizeof(struct block_desc)
/* Helper macro. The end of the using memory pool*/
#define LAST_ADDRESS     (memory + (uint32_t)sizeof(memory))

/* This is a pointer to a current free memory.
 * We start to search appropriate memory block for allocation
 * from this address */
static char *current_space;

/* This is our pool memory, that we allocated,
 * size defined from pool_size option. */
static char memory[OPTION_GET(NUMBER,pool_size)];

/*This function check address whether it from our pool or not */
static int correct_address(char *address) {
	return (((uint32_t) address >= (uint32_t) memory)
			&& ((uint32_t) address < (uint32_t) LAST_ADDRESS));
}

/* This function get  status of the memory block
 * 1 - block is free, can be allocated
 * 0 - block is busy, used now */
static int get_available(struct block_desc *md) {
	return (md->is_available);
}

/* This function marks the memory block as free (available) */
static void set_available(struct block_desc *block) {
	block->is_available = 1;
}

/* This function set field "available" not available */
static void set_not_available(struct block_desc *block) {
	block->is_available = 0;
}

/* This function checks whether the block has suit size for allocation*/
static int suit_size(struct block_desc *md, size_t req_size) {
	return ((uint32_t) md->size
			>= (uint32_t) req_size + (uint32_t) BLOCK_DESC_SIZE);
}

/**
 * This function searches an available block with appropriate size.
 */
static struct block_desc *find_suit_block(size_t req_size) {
	/* Set the pointer(iterator) on the begin of our free memory */
	struct block_desc *md = (void *) current_space;

	printf("looking for: %d\n", req_size);
#if 0
	printf("current_free_space 0x%x\n", (uint32_t)current_space);
#endif

	/*Check address correction*/
	if (correct_address(current_space) == 0) {
		printf("address 0x%X is not correct!!!  \n", (uint32_t) current_space);
		return NULL;
	}
	/* We iterates under our pool while current block is not available
	 * or req_size of current block less then req_size.
	 * If the iterator exceed the end of the our available memory we couldn't
	 *  allocate appropriate block and return NULL */
	while ((!get_available(md)) || (!suit_size(md, req_size))) {
		md = (void *) (((size_t) md) + md->size);
#if 0
		printf("md = 0x%X\n", (uint32_t)md);
		printf("block is not able for this new block \n");
#endif
		/*Check the end of the memory*/
		if (((uint32_t) md) >= (uint32_t) (memory) + sizeof(memory)) {
			return NULL;
		}
	}
#if 0
	printf("need %d\n", (uint32_t)(req_size) + (uint32_t)(BLOCK_DESC_SIZE));
	printf("find 0x%x\n", (uint32_t)md);
#endif
	return md;
}

/**
 * This is a function, which allocates a block with necessary @size.
 *
 * @size - size of needed block;
 * @return - pointer to the allocated memory; */
static void *memory_allocate(size_t req_size) {
	/* allocated part */
	struct block_desc *new_block;
	/* another part of existing block */
	struct block_desc *old_block;

	/*If suit block is not exist, allocation is impossible*/
	if (NULL == (new_block = find_suit_block(req_size))) {
		return NULL;
	}

	/* Change state flag on unavailable*/
	set_not_available(new_block);
	/*If  old block less then BLOCK_DESC_SIZE don't to cut it*/
	if ((new_block->size - req_size - BLOCK_DESC_SIZE) >= BLOCK_DESC_SIZE) {
		/* Initializes a new block on the remaining part of block */
		old_block = (void *) ((size_t) new_block + req_size + BLOCK_DESC_SIZE);
		set_available(old_block);
		old_block->size = new_block->size - req_size - BLOCK_DESC_SIZE;
		/* Fixed req_size of block */
		new_block->size = req_size + BLOCK_DESC_SIZE;
	}

	/*If select min free space, change enter to search */
	if (current_space == (void*) new_block) {
		current_space += (uint32_t) (new_block->size); /* check types*/
	}

	printf("returned 0x%x\n",
			(uint32_t) (new_block) + (uint32_t) (BLOCK_DESC_SIZE));
	printf("current_free_space 0x%x\n", (uint32_t) current_space);

	/*Return point to memory in block*/
	return (void *) ((uint32_t) (new_block) + (uint32_t) (BLOCK_DESC_SIZE));
}

/*Resolve defragmentation with a next block*/
static void defragmentation(struct block_desc *md) {
	struct block_desc *next_md;

	next_md = (void *) (((size_t) md) + md->size);

	if (!correct_address((char *) md)) {
		return;
	}

	if (get_available(md)) {
		/*Look at the next block and it is free, paste it*/
		if (get_available(next_md)) {
			md->size = md->size + next_md->size;
			defragmentation(md);
		}
	}
	defragmentation(next_md);
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
	defragmentation(md);

	printf("NEW current_free_space 0x%x\n", (uint32_t) current_space);
}

/* This is procedure of the beginning
 * initialization memory. */
static void memory_init(void) {
	struct block_desc *md;

	/*Set search pointer to memory start */
	current_space = memory;

	printf("start of memory = 0x%X\n", (uint32_t) memory);
	printf("end of memory = 0x%X\n",
			((uint32_t) memory) + (uint32_t) sizeof(memory));
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
static void input_mem(void) {
	int p = 0;
	struct block_desc *md;
	md = (void *) memory;

	while (correct_address((char *) md)) {
		printf("block num = %d\n ", p);
		printf("   address = 0x%X\n", (uint32_t) md);
		printf("   available = %d\n", get_available(md));
		printf("   size = 0x%X\n", (uint32_t) md->size);
		md = (void *) (((size_t) md) + md->size);
		p++;
	}
}

/**/
static void example(void) {
	int i, j;
	/* Just a synthetic example*/
	/*Allocation 18 times*/
	int number_add_block = 18, I_alloc = 9, II_alloc = 8;
	uint32_t k = 0x100; //!!!!!!
	void *succes_alloc[number_add_block];
	int koef_alloc[] = {
			/*First allocation:  I_alloc = 9*/1, 2, 3, 1, 4, 8, 3, 2, 5,
			/*Second allocation: II_alloc = 8*/7, 4, 1, 2, 1, 1, 6, 267,
			/*Third allocation:  III_alloc = 1*/16 };
	/*Free 12 block*/
	int I_free = 5, II_free = 7;
	int number_of_block[] = {
			/*First free:  I_free = 5*/3, 6, 2, 4, 8,
			/*Second free: II_free = 7*/12, 1, 7, 14, 15, 5, 10 };

	/*First allocation*/
	for (i = 0; i < I_alloc; i++) {
		printf("==NEW BLOCK==\n");
		succes_alloc[j] = memory_allocate(koef_alloc[j] * k - 8);
		if (succes_alloc[j] == NULL) {
			printf(
					"\nMemory allocation error on the addition %d size of block: %d\n",
					j, koef_alloc[j] * k);
		} else {
			printf("block num = %d address = 0x%X\n\n", j,
					(uint32_t) succes_alloc[j]);
		}
		j++;
	}

	/*First memory free*/
	for (i = 0; i < I_free; i++) {
		memory_free(succes_alloc[number_of_block[i] - 1]);
		printf("free block num = %d address = 0x%X\n\n", i,
				(uint32_t) succes_alloc[i]);
	}

	/*Second allocation*/
	for (i = 0; i < II_alloc; i++) {
		printf("==NEW BLOCK==\n");
		if (j == 13) {
			succes_alloc[j] = memory_allocate(koef_alloc[j] * k - 8 - 8);
		} else {
			if (j == 14) {
				succes_alloc[j] = memory_allocate(koef_alloc[j] * k - 8 - 3);
			} else {
				succes_alloc[j] = memory_allocate(koef_alloc[j] * k - 8);
			}
		}
		if (succes_alloc[j] == NULL) {
			printf(
					"\nMemory allocation error on the addition %d size of block: %d\n",
					j, koef_alloc[j] * k);
		} else {
			printf("block num = %d address = 0x%X\n\n", j,
					(uint32_t) succes_alloc[j]);
		}
		j++;
	}

	/*Second memory free*/
	for (i = I_free; i < I_free + II_free; i++) {
		memory_free(succes_alloc[number_of_block[i] - 1]);
		printf("free block num = %d address = 0x%X\n\n", i,
				(uint32_t) succes_alloc[i]);
	}

	/*Third allocation*/
	printf("==NEW BLOCK==\n");
	succes_alloc[j] = memory_allocate(koef_alloc[j] * k - 8);
	if (succes_alloc[j] == NULL) {
		printf(
				"\nMemory allocation error on the addition %d size of block: %d\n",
				j, koef_alloc[j] * k);
	} else {
		printf("block num = %d address = 0x%X\n\n", j,
				(uint32_t) succes_alloc[j]);
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
		if (rand() % 2 == 0) {
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
