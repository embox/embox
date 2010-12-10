/**
 * @file
 *
 * @brief Pseudo-dynamic memory allocator.
 *
 * @date 13.10.09
 * @author Nikolay Korotky
 */
#include <embox/kernel.h>
#include <string.h>

#define MEM_POOL_SIZE 0x11CA

static int has_initialized = 0;
static char *managed_memory_start;
static char *last_valid_address;

static char mem_pool[MEM_POOL_SIZE];

struct mem_control_block {
	char is_available;
	size_t size;
};

static void malloc_init(void) {
	struct mem_control_block *init_mcb;
	managed_memory_start = (void*) mem_pool;
	last_valid_address   = managed_memory_start;
	has_initialized      = 1;

	init_mcb = (struct mem_control_block *) managed_memory_start;
	init_mcb->is_available = 1;
	init_mcb->size = sizeof(mem_pool) - sizeof(struct mem_control_block);
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

/*FIXME ATTENTION: memory size must be aligned!*/
void *malloc(size_t size) {
	char *current_location;
	struct mem_control_block *current_location_mcb;
	char *memory_location = NULL;
	if (!has_initialized) {
		malloc_init();
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
			LOG_ERROR("mem_pool is full!\n");
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
	char *tmp = malloc(size);
	struct mem_control_block *mcb;
	mcb = (void *) ((char *) ptr - sizeof(struct mem_control_block));
	memcpy(tmp, ptr, mcb->size - sizeof(struct mem_control_block));
	free(ptr);
	return tmp;
}
