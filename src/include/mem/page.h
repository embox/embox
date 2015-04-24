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
#include <stdint.h>

#include <framework/mod/options.h>

#include <module/embox/mem/page_api.h>

struct page_allocator {
	void *pages_start;
	unsigned int pages_n;
	size_t page_size;

	size_t free;

	size_t bitmap_len;
	unsigned long *bitmap;
};

extern struct page_allocator *page_allocator_init(char *start, size_t len, size_t page_size);

extern void *page_alloc(struct page_allocator *allocator, size_t page_number);

extern void *page_alloc_zero(struct page_allocator *allocator, size_t page_number);

extern void page_free(struct page_allocator *allocator, void *page, size_t page_number);

extern int page_belong(struct page_allocator *allocator, void *page);

#define PAGE_ALLOCATOR_DEF(name, space, page_number, page_size) \
	static unsigned long ctrl_space_##name[ page_number/32 ]; \
	static struct page_allocator name = { \
			space, \
			page_number, \
			page_size, \
			page_size * page_number, \
			(sizeof(unsigned long) * page_number/32), \
			ctrl_space_##name \
	}

#endif /* MEM_PAGE_H_ */
