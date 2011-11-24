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

#include <types.h>

#define PAGE_SIZE() CONFIG_PAGE_SIZE

extern void *page_alloc(size_t page_number);

extern void page_free(void *page, size_t page_number);

#endif /* MEM_PAGE_H_ */
