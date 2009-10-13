/**
 * \file malloc.c
 * \date 13.10.09
 * \author Sikmir
 * \description Static memory allocator
 */
#include "conio.h"

#define MEM_POOL_SIZE 0x11CA

int has_initialized = 0;
void *managed_memory_start;
void *last_valid_address;

static char mem_pool[MEM_POOL_SIZE];

struct mem_control_block {
        int is_available;
        int size;
};

void malloc_init() {
	last_valid_address   = (void*)mem_pool;
	managed_memory_start = last_valid_address;
	has_initialized      = 1;
}

void free(void *ptr) {
	struct mem_control_block *mcb;

	mcb = ptr - sizeof(struct mem_control_block);
	mcb->is_available = 1;
	return;
}

void *malloc(long size) {
	void *current_location;
        struct mem_control_block *current_location_mcb;
	void *memory_location;

	if(! has_initialized) {
		malloc_init();
	}

	size = size + sizeof(struct mem_control_block);
	memory_location = 0;
	current_location = managed_memory_start;

	while(current_location != last_valid_address) {
		current_location_mcb = (struct mem_control_block *)current_location;
		if(current_location_mcb->is_available) {
			if(current_location_mcb->size >= size) {
				current_location_mcb->is_available = 0;
				memory_location = current_location;
				break;
			}
		}
		current_location = current_location + current_location_mcb->size;
	}

	if(! memory_location) {
		memory_location = last_valid_address;
		last_valid_address = last_valid_address + size;
		current_location_mcb = memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size = size;
	}

	memory_location = memory_location + sizeof(struct mem_control_block);

	return memory_location;
}
