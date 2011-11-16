/**
 * @file
 *
 * @brief
 *
 * @date 14.11.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <util/array.h>


/* heap_start _heap_end */
static uint32_t bitmask[((CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE)/32) + 1];

void *physmem_page_alloc(void) {
	extern char *_heap_start;
	int word_offset;
	int bit_offset;
	uint32_t word;
	uint32_t mask;
	void *page;

	for(word_offset = 0; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		word = bitmask[word_offset];
		mask = 1;
		for(bit_offset = 0; bit_offset < 32; ++bit_offset) {
			//TODO spinlock
			if(0 == (word & mask)) { /*page is free*/
				bitmask[word_offset] |= mask;
				page = (void*) (_heap_start + CONFIG_PAGE_SIZE * ((word_offset << 5) + bit_offset));
			}
		}
	}
	return page;
}

void physmem_page_free(void) {

}
