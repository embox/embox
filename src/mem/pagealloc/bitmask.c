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



//TODO page : have no synchronization

extern char *_heap_start; //TODO page : calculate free memory dynamic
#define FREE_MEM_BASE (uint32_t)&_heap_start
/* _heap_start _heap_end */
static uint32_t bitmask[((CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE)/32) + 1];

#define REST_MASK_BIT     (32 - (CONFIG_HEAP_SIZE/CONFIG_PAGE_SIZE) / 32)

static void *search_single_page(void) {
	int word_offset;
	int bit_offset;
	uint32_t word;
	uint32_t mask;
	void *page = NULL;

	for (word_offset = 0; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		word = bitmask[word_offset];
		mask = 1;
		if(word == -1) {
			continue;
		}
		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {

			if (0 == (word & mask)) { /* page is free */
				bitmask[word_offset] |= mask;
				page = (void*) (FREE_MEM_BASE + CONFIG_PAGE_SIZE
						* ((word_offset << 5) + bit_offset));
				return page;
			}
		}
	}
	if (REST_MASK_BIT) {
		word = bitmask[ARRAY_SIZE(bitmask) - 1];
		mask = 1;
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if (0 == (word & mask)) { /* page is free */
				bitmask[word_offset] |= mask;
				page = (void*) (FREE_MEM_BASE + CONFIG_PAGE_SIZE
						* ((word_offset << 5) + bit_offset));
			}
		}
	}

	return page;
}

static size_t search_first_free(size_t start_page) {
	uint32_t word;
	size_t word_offset = start_page / 32;
	int bit_offset = start_page % 32;
	//uint32_t mask;

	if (0 != bit_offset) {
		word = bitmask[word_offset];
		//mask = 1 << bit_offset;

		for (; bit_offset < 32; bit_offset++) {
			if (0 == (word & (1 << bit_offset))) { /* page is free */
				return ((word_offset * 32) + bit_offset);
			}
		}
		word_offset ++;
	}

	for (; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		word = bitmask[word_offset];
		//mask = 1;
		if (word == -1) {
			continue;
		}
		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {
			if (0 == (word & (1 << bit_offset))) { /* page is free */
				return ((word_offset * 32) + bit_offset);
			}
		}
	}

	if (REST_MASK_BIT) {
		word = bitmask[ARRAY_SIZE(bitmask) - 1];
		//mask = 1;
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if (0 == (word & (1 << bit_offset))) { /* page is free */
				return ((word_offset * 32) + bit_offset);
			}
		}
	}
	return -1;
}

static size_t check_n_free(size_t start_page, size_t page_q) {
	uint32_t word;
	size_t word_offset = start_page / 32;
	int bit_offset = start_page % 32;
	//uint32_t mask;
	size_t page_cnt = page_q;

	if (0 != bit_offset) {
		word = bitmask[word_offset];
		//mask = 1 << bit_offset;

		for (; bit_offset < 32; bit_offset++) {
			if (0 == (word & (1 << bit_offset)) && (0 == --page_cnt)) { /* page is free */
				return page_q;
			}
			if (0 != (word & (1 << bit_offset))) {
				page_cnt = page_q;
			}
		}
		word_offset ++;
	}

	for (; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		word = bitmask[word_offset];
		//mask = 1;

		if (word == -1) {
			page_cnt = page_q;
			continue;
		}

		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {
			if (0 == (word & (1 << bit_offset)) && (0 == --page_cnt)) { /* page is free */
				return page_q;
			}
			if (0 != (word & (1 << bit_offset))) {
				page_cnt = page_q;
			}
		}
	}

	if (REST_MASK_BIT) {
		word = bitmask[ARRAY_SIZE(bitmask) - 1];
		//mask = 1;

		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if (0 == (word & (1 << bit_offset)) && (0 == --page_cnt)) { /* page is free */
				return page_q;
			}
			if (0 != (word & (1 << bit_offset))) {
				page_cnt = page_q;
			}
		}
	}
	return (page_q - page_cnt);
}

static void mark_n_busy(size_t page_n, size_t page_q) {
	size_t word_offset = page_n / 32;
	int bit_offset = page_n % 32;

	if (0 != bit_offset) {
		for (; bit_offset < 32; ++bit_offset) {
			if(page_q--) {
				bitmask[word_offset] |= 1 << bit_offset;
			} else {
				return ;
			}
		}
		word_offset ++;
	}

	for (; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {
			if(page_q--) {
				bitmask[word_offset] |= 1 << bit_offset;
			} else {
				return ;
			}
		}
	}

	if (REST_MASK_BIT) {
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if(page_q--) {
				bitmask[word_offset] |= 1 << bit_offset;
			} else {
				return ;
			}
		}
	}
}

static void mark_n_free(size_t page_n, size_t page_q) {
	size_t word_offset = page_n / 32;
	int bit_offset = page_n % 32;

	if (0 != bit_offset) {
		for (; bit_offset < 32; ++bit_offset) {
			if(page_q--) {
				bitmask[word_offset] &= ~(1 << bit_offset);
			} else {
				return;
			}
		}
		word_offset ++;
	}

	for (; word_offset < ARRAY_SIZE(bitmask) - 1; ++word_offset) {
		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {
			if(page_q--) {
				bitmask[word_offset] &= ~(1 << bit_offset);
			} else {
				return;
			}
		}
	}

	if (REST_MASK_BIT) {
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if(page_q--) {
				bitmask[word_offset] &= ~(1 << bit_offset);
			} else {
				return;
			}
		}
	}
}

static void *search_multi_page(size_t page_q) {
	size_t page_n = 0;
	size_t found_page_q;

	while(-1 != (page_n = search_first_free(page_n))) {
		if(page_q == (found_page_q = check_n_free(page_n, page_q))) {
			mark_n_busy(page_n, page_q);
			return (void*) (FREE_MEM_BASE + CONFIG_PAGE_SIZE * page_n);
		}
		page_n += found_page_q;
	}

	return NULL;
}

void *page_alloc(size_t page_q) {
	if (1 == page_q) {
		return search_single_page();
	}

	return search_multi_page(page_q);
}

void page_free(void *page, size_t page_q) {
	int word_offset;
	int bit_offset;
	size_t page_n;

	page_n = ((uint32_t)page - (uint32_t)FREE_MEM_BASE) / CONFIG_PAGE_SIZE;
	word_offset = page_n / 32;
	bit_offset = page_n % 32;


	if(1 == page_q) {
		bitmask[word_offset] &= ~(1 << bit_offset);
		return;
	}
	mark_n_free(page_n, page_q);
}

static int page_alloc_init(void) {
	return 0;
}
