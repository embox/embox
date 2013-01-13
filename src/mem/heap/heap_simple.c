/**
 * @file
 * @brief Pseudo-dynamic memory allocator.
 *
 * @date 13.10.09
 * @author Nikolay Korotky
 */


#include <string.h>
#include <stdlib.h>
#include <embox/unit.h>
#include <mem/heap.h>
#include <mem/page.h>

static int has_initialized = 0;
static char *managed_memory_start;
static char *last_valid_address;

#define HEAP_SIZE OPTION_MODULE_GET(embox__mem__heap_api,NUMBER,heap_size)

#define MEM_POOL_SIZE  (HEAP_SIZE / PAGE_SIZE())
static void *mem_pool;

struct mem_control_block {
	int is_available;
	size_t size;
};

static int malloc_init(void) {
	extern struct page_allocator *__heap_pgallocator;
	struct mem_control_block *init_mcb;

	mem_pool = page_alloc(__heap_pgallocator, MEM_POOL_SIZE);
	if(NULL == mem_pool) {
		return -1;
	}

	managed_memory_start = (void*) mem_pool;
	last_valid_address   = managed_memory_start;
	has_initialized      = 1;

	init_mcb = (struct mem_control_block *) managed_memory_start;
	init_mcb->is_available = 1;
	init_mcb->size = sizeof(mem_pool) - sizeof(struct mem_control_block);

	return 0;
}

static void _mem_defrag(void) {
	char *current_location, *next;
	struct mem_control_block *current_location_mcb, *next_mcb;
	current_location = managed_memory_start;
	current_location_mcb = (struct mem_control_block *) current_location;
	while (current_location_mcb->size) {
		if (current_location_mcb->is_available == 1) {
			next = current_location + current_location_mcb->size;
			next_mcb = (struct mem_control_block *) next;
			if (next_mcb->is_available == 1) {
				current_location_mcb->size += next_mcb->size;
			}
		}
		current_location += current_location_mcb->size;
		current_location_mcb = (struct mem_control_block *) current_location;
	}
}

void free(void *ptr) {
	struct mem_control_block *mcb;
	mcb = (void *)((char *) ptr - sizeof(struct mem_control_block));
	mcb->is_available = 1;
	 _mem_defrag();
	return;
}

void *malloc(size_t size) {
	char *current_location;
	struct mem_control_block *current_location_mcb;
	char *memory_location = NULL;
	if (!has_initialized) {
		if(-1 == malloc_init())
			return NULL;
	}
	current_location = managed_memory_start;
	size += sizeof(struct mem_control_block);

	while (current_location != last_valid_address) {
		current_location_mcb = (struct mem_control_block *) current_location;
		if (current_location_mcb->is_available) {
			if (current_location_mcb->size >= size) {
				current_location_mcb->is_available = 0;
				memory_location = current_location;
				break;
			}
		}
		current_location += current_location_mcb->size;
	}
	if (!memory_location) {
		memory_location = last_valid_address;
		last_valid_address += size;
		if (last_valid_address > managed_memory_start + MEM_POOL_SIZE) {
			return NULL;
		}
		current_location_mcb = (struct mem_control_block *) memory_location;
		current_location_mcb->is_available = 0;
		current_location_mcb->size	 = size;
	}
	memory_location += sizeof(struct mem_control_block);
	return memory_location;
}

void *calloc(size_t nmemb, size_t size) {
	void *tmp = malloc(nmemb * size);
	memset(tmp, 0, nmemb * size);
	return tmp;
}

void *realloc(void *ptr, size_t size) {
	struct mem_control_block *mcb;
	char *tmp = malloc(size);
	if (ptr == NULL) {
		return tmp;
	}
	mcb = (void *) ((char *) ptr - sizeof(struct mem_control_block));
	memcpy(tmp, ptr, mcb->size - sizeof(struct mem_control_block));
	free(ptr);
	return tmp;
}
