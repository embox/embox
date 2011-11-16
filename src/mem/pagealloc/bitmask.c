/**
 * @file
 *
 * @brief
 *
 * @date 14.11.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <embox/unit.h>
#include <util/array.h>

#include <mem/page.h>

EMBOX_UNIT_INIT(page_alloc_init);

/* _heap_start _heap_end */
static uint32_t bitmask[((CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE)/32) + 1];

#define REST_MASK_BIT     (32 - (CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE) / 32)

extern char *_heap_start; //TODO calculate free memory dynamic


#define PHY_MEM_BASE _heap_start

static void *search_single_page(void) {
	int word_offset;
	int bit_offset;
	uint32_t word;
	uint32_t mask;
	void *page = NULL;

	for (word_offset = 0; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		word = bitmask[word_offset];
		mask = 1;
		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {
			//TODO spinlock
			if (0 == (word & mask)) { /* page is free */
				bitmask[word_offset] |= mask;
				page = (void*) (PHY_MEM_BASE + CONFIG_PAGE_SIZE
						* ((word_offset << 5) + bit_offset));
			}
		}
	}
	if (REST_MASK_BIT) {
		word = bitmask[ARRAY_SIZE(bitmask) - 1];
		mask = 1;
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if (0 == (word & mask)) { /* page is free */
				bitmask[word_offset] |= mask;
				page = (void*) (PHY_MEM_BASE + CONFIG_PAGE_SIZE
						* ((word_offset << 5) + bit_offset));
			}
		}
	}

	return page;
}

void *page_alloc(size_t page_number) {
	if (1 == page_number) {
		return search_single_page();
	}

	return NULL;
}

void page_free(void *page, size_t page_number) {
	int word_offset;
	int bit_offset;

	word_offset = (((uint32_t)PHY_MEM_BASE - (uint32_t)page) / CONFIG_PAGE_SIZE) / 32;
	bit_offset = (((uint32_t)PHY_MEM_BASE - (uint32_t)page) / CONFIG_PAGE_SIZE) % 32;

	bitmask[word_offset] &= ~(1 << bit_offset);
}

static int page_alloc_init(void) {
	return 0;
}
