/**
 * @file
 *
 * @brief
 *
 * @date 14.11.2011
 * @author Anton Bondarev
 */

#include <embox/unit.h>
#include <util/array.h>
#include <stdint.h>
#include <stddef.h>

#include <mem/page.h>
#include <mem/heap.h>

#include <string.h>



//TODO page : have no synchronization

#define FREE_MEM_BASE       ((uint32_t)allocator->start)
#define bitmask             ((uint32_t *) (allocator->start + sizeof(struct page_allocator)))
#define MAIN_BITMASK_LENGTH (allocator->capacity / allocator->page_size) / 32
#define REST_MASK_BIT       (allocator->capacity / allocator->page_size) % 32

static void *search_single_page(struct page_allocator *allocator) {
	int word_offset;
	int bit_offset;
	uint32_t word;
	uint32_t mask;
	void *page = NULL;

	for (word_offset = 0; word_offset < MAIN_BITMASK_LENGTH; ++word_offset) {
		word = bitmask[word_offset];
		mask = 1;
		if(word == -1) {
			continue;
		}
		for (bit_offset = 0; bit_offset < 32; ++bit_offset, mask <<= 1) {

			if (0 == (word & mask)) { /* page is free */
				bitmask[word_offset] |= mask;
				page = (void*) (FREE_MEM_BASE + PAGE_SIZE()
						* ((word_offset << 5) + bit_offset));
				return page;
			}
		}
	}
	if (REST_MASK_BIT) {
		word = bitmask[MAIN_BITMASK_LENGTH - 1];
		mask = 1;
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if (0 == (word & mask)) { /* page is free */
				bitmask[word_offset] |= mask;
				page = (void*) (FREE_MEM_BASE + PAGE_SIZE()
						* ((word_offset << 5) + bit_offset));
			}
		}
	}

	return page;
}

static size_t search_first_free(struct page_allocator *allocator, size_t start_page) {
	uint32_t word;
	size_t word_offset = start_page / 32;
	int bit_offset = start_page % 32;
#if 0
	if(HEAP_SIZE()/PAGE_SIZE() <= start_page) {
		return -1;
	}
#endif
	if((allocator->capacity / allocator->page_size) <= start_page) {
		return -1;
	}

	if (0 != bit_offset) {
		word = bitmask[word_offset];
		//mask = 1 << bit_offset;

		for (; bit_offset < 32; bit_offset++) {
			if (0 == (word & (1 << bit_offset))) { /* page is free */
				allocator->free -= allocator->page_size;
				return ((word_offset * 32) + bit_offset);
			}
		}
		word_offset ++;
	}

	for (; word_offset < MAIN_BITMASK_LENGTH; ++word_offset) {
		word = bitmask[word_offset];
		//mask = 1;
		if (word == -1) {
			continue;
		}
		for (bit_offset = 0; bit_offset < 32; ++bit_offset) {
			if (0 == (word & (1 << bit_offset))) { /* page is free */
				allocator->free -= allocator->page_size;
				return ((word_offset * 32) + bit_offset);
			}
		}
	}

	if (REST_MASK_BIT) {
		word = bitmask[MAIN_BITMASK_LENGTH - 1];
		//mask = 1;
		for (bit_offset = 0; bit_offset < REST_MASK_BIT; ++bit_offset) {
			if (0 == (word & (1 << bit_offset))) { /* page is free */
				allocator->free -= allocator->page_size;
				return ((word_offset * 32) + bit_offset);
			}
		}
	}
	return -1;
}

static size_t check_n_free(struct page_allocator *allocator, size_t start_page, size_t page_q) {
	uint32_t word;
	size_t word_offset = start_page / 32;
	int bit_offset = start_page % 32;
	size_t page_cnt = page_q;

	if (0 != bit_offset) {
		word = bitmask[word_offset];

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

	for (; word_offset < MAIN_BITMASK_LENGTH; ++word_offset) {
		word = bitmask[word_offset];

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
		word = bitmask[MAIN_BITMASK_LENGTH - 1];

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

static void mark_n_busy(struct page_allocator *allocator, size_t page_n, size_t page_q) {
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

	for (; word_offset < MAIN_BITMASK_LENGTH; ++word_offset) {
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

static void mark_n_free(struct page_allocator *allocator, size_t page_n, size_t page_q) {
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

	for (; word_offset < MAIN_BITMASK_LENGTH; ++word_offset) {
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

static void *search_multi_page(struct page_allocator *allocator, size_t page_q) {
	size_t page_n = 0;
	size_t found_page_q;

	while(-1 != (page_n = search_first_free(allocator, page_n))) {
		if(page_q == (found_page_q = check_n_free(allocator, page_n, page_q))) {
			mark_n_busy(allocator, page_n, page_q);
			allocator->free -= page_q * allocator->page_size;
			return (void*) (FREE_MEM_BASE + PAGE_SIZE() * page_n);
		}
		page_n += found_page_q;
	}

	return NULL;
}

void *page_alloc(struct page_allocator *allocator, size_t page_q) {
	if (1 == page_q) {
		return search_single_page(allocator);
	}

	return search_multi_page(allocator, page_q);
}

void *page_alloc_zero(struct page_allocator *allocator, size_t page_q) {
	char *page_p;

	if(NULL != (page_p = page_alloc(allocator, page_q))) {
		memset(page_p, 0, PAGE_SIZE());
	}

	return page_p;
}

void page_free(struct page_allocator *allocator, void *page, size_t page_q) {
	int word_offset;
	int bit_offset;
	size_t page_n;

	page_n = ((uint32_t)page - (uint32_t)FREE_MEM_BASE) / PAGE_SIZE();
	word_offset = page_n / 32;
	bit_offset = page_n % 32;

	allocator->free += page_q * allocator->page_size;

	if(1 == page_q) {
		bitmask[word_offset] &= ~(1 << bit_offset);
		return;
	}
	mark_n_free(allocator, page_n, page_q);
}

struct page_allocator *page_allocator_init(char *start, size_t len, size_t page_size) {
	struct page_allocator *allocator;
	int pages;
	int idx;

	if(len < page_size) {
		return NULL;
	}

	allocator = (struct page_allocator *)start;
	pages = len / page_size;

	memset(allocator, 0, page_size);
	allocator->start = start;
	allocator->capacity = len;
	allocator->page_size = page_size;
	allocator->free = len - page_size; /* one page for service information */
	mark_n_busy(allocator, 0, 1); /* mark first page as busy */
	/* now we have allocator with bitmask for
	 * n = (page_size - sizeof(struct page_allocator))  * 8 pages
	 */
	pages -= (page_size - sizeof(struct page_allocator))  * 8;
	idx = 1;
	while(pages > 0) {
		memset(allocator->start + page_size * idx, 0, page_size);
		mark_n_busy(allocator, idx, 1);

		idx++;
		allocator->free -= page_size;
		pages -= page_size * 8;
	}

	return allocator;
}
