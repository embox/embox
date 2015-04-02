/**
 * @file
 *
 * @date Oct 5, 2012
 * @author: Anton Bondarev
 */

#ifndef MEM_PHYMEM_H_
#define MEM_PHYMEM_H_

#include <mem/page.h>

extern struct page_allocator *__phymem_allocator;

#define __phymem_end (__phymem_allocator->pages_start + \
		(__phymem_allocator->page_size * __phymem_allocator->pages_n))

extern void *phymem_alloc(size_t page_number);
extern void phymem_free(void *page, size_t page_number);

#endif /* MEM_PHYMEM_H_ */
