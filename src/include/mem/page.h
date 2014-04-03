/**
 * @file
 *
 * @brief
 *
 * @date 16.11.2011
 * @author Anton Bondarev
 */

#ifndef MEM_PAGE_H_
#define MEM_PAGE_H_

#include <stddef.h>
#include <framework/mod/options.h>

#include <module/embox/mem/page_api.h>

struct page_allocator {
	void *pages_start;
	unsigned int pages_n;
	size_t page_size;

	size_t free;

	unsigned long bitmap[];
};

extern struct page_allocator *page_allocator_init(char *start, size_t len, size_t page_size);

extern void *page_alloc(struct page_allocator *allocator, size_t page_number);

extern void *page_alloc_zero(struct page_allocator *allocator, size_t page_number);

extern void page_free(struct page_allocator *allocator, void *page, size_t page_number);

#endif /* MEM_PAGE_H_ */
