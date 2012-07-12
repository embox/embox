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
#include <framework/mod/options.h>

#if 0
#define PAGE_SIZE() CONFIG_PAGE_SIZE
#endif
#include <module/embox/mem/page_api.h>
#define PAGE_SIZE() OPTION_MODULE_GET(embox__mem__page_api,NUMBER,page_size)


extern void *page_alloc(size_t page_number);

extern void page_free(void *page, size_t page_number);

#endif /* MEM_PAGE_H_ */
