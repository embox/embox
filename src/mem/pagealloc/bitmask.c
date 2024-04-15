/**
 * @file
 *
 * @brief
 *
 * @date 14.11.2011
 * @author Anton Bondarev
 * @author Anton Kozlov
 */

#include <util/log.h>

#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include <util/binalign.h>
#include <lib/libds/bitmap.h>
#include <util/math.h>

#include <mem/page.h>

//TODO page : have no synchronization

static inline int page_ptr2i(struct page_allocator *allocator, void *page) {
	return (page - allocator->pages_start) / allocator->page_size;
}

static inline void *page_i2ptr(struct page_allocator *allocator, int i) {
	return allocator->pages_start + i * allocator->page_size;
}

static unsigned int search_first_free(struct page_allocator *allocator,
		unsigned int start_page) {
	unsigned int res;

	res = bitmap_find_zero_bit(allocator->bitmap, allocator->pages_n, start_page);

	return res != allocator->pages_n ? res : -1;
}

static unsigned int check_n_free(struct page_allocator *allocator,
		unsigned int start_page, unsigned int page_q) {
	assert(allocator);
	const int nbits = min(start_page + page_q, allocator->pages_n);
	return bitmap_find_bit(allocator->bitmap, nbits, start_page) - start_page;
}

static void mark_n_busy(struct page_allocator *allocator,
		unsigned int start_page, unsigned int page_q) {
	unsigned int page_i;

	assert(allocator);
	assert(start_page + page_q <= allocator->pages_n);
	for (page_i = start_page; page_i < start_page + page_q; page_i++) {
		bitmap_set_bit(allocator->bitmap, page_i);
	}

	allocator->free -= page_q * allocator->page_size;
}

static void mark_n_free(struct page_allocator *allocator,
		unsigned int start_page, unsigned int page_q) {
	unsigned int page_i;
	int count_free = 0;

	assert(allocator);
	assert(start_page + page_q <= allocator->pages_n);
	for (page_i = start_page; page_i < start_page + page_q; page_i++) {
		if (bitmap_test_bit(allocator->bitmap, page_i)) {
			bitmap_clear_bit(allocator->bitmap, page_i);
			++count_free;
		}
	}

	allocator->free += count_free * allocator->page_size;
}

static void *search_multi_page(struct page_allocator *allocator, size_t page_q) {
	size_t page_n = 0;
	size_t found_page_q;

	while (-1 != (page_n = search_first_free(allocator, page_n))) {
		if (page_q == (found_page_q = check_n_free(allocator, page_n, page_q))) {
			mark_n_busy(allocator, page_n, page_q);
			return page_i2ptr(allocator, page_n);
		}
		page_n += found_page_q;
	}

	return NULL;
}

void *page_alloc(struct page_allocator *allocator, size_t page_q) {
	assert(allocator);

	return search_multi_page(allocator, page_q);
}

void *page_alloc_zero(struct page_allocator *allocator, size_t page_q) {
	char *page_p;

	if (NULL != (page_p = page_alloc(allocator, page_q))) {
		memset(page_p, 0, page_q * allocator->page_size);
	}

	return page_p;
}

void page_free(struct page_allocator *allocator, void *page, size_t page_q) {
	mark_n_free(allocator, page_ptr2i(allocator, page), page_q);
}

struct page_allocator *page_allocator_init(char *start, size_t len, size_t page_size) {
	char *pages_start;
	struct page_allocator *allocator;
	unsigned int pages;
	size_t bitmap_len;

	if (len < page_size + sizeof(struct page_allocator)) {
		return NULL;
	}

	start = (char *) binalign_bound((uintptr_t) start, 16);
	pages = len / page_size;
	pages_start = (char *) binalign_bound((uintptr_t) start, page_size);

	bitmap_len = sizeof(unsigned long) * BITMAP_SIZE(pages + 1); /* one for guardbit */

	while (sizeof(struct page_allocator) + bitmap_len > pages_start - start) {
		pages_start += page_size;
		pages--;
		if (pages <= 0) {
			return NULL;
		}
	}


	allocator = (struct page_allocator *) start;
	log_debug("allocator(%p)", allocator);

	allocator->pages_start = pages_start;
	allocator->pages_n = pages;
	allocator->page_size = page_size;
	allocator->free = pages * page_size;
	allocator->bitmap_len = bitmap_len;
	allocator->bitmap = (unsigned long *)((uintptr_t)&allocator->bitmap + sizeof(allocator->bitmap));

	log_debug("allocator->bitmap(%p) bitmap_len(%=%zu)", allocator->bitmap, bitmap_len);

	memset(allocator->bitmap, 0, bitmap_len);
	bitmap_set_bit(allocator->bitmap, pages);

	return allocator;
}

int page_belong(struct page_allocator *allocator, void *page) {
	void *pages_end = allocator->pages_start + allocator->pages_n * allocator->page_size;
	return allocator->pages_start <= page && page < pages_end;
}
