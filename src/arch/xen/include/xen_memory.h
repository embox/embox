/**
 * @file    xen_memory.h
 * @brief
 *
 * @author  Aleksei Cherepanov
 * @date    30.03.2020
 */

#ifndef XEN_MEMORY_
#define XEN_MEMORY_

#include <unistd.h>
#include <xen/xen.h>
#include <mem/page.h>

#define PAGE_SHIFT      12

extern void *xen_mem_alloc(size_t page_number);
extern void xen_mem_free(void *page, size_t page_number);
extern long int virt_to_mfn(void* virt);

#endif /* XEN_MEMORY_ */
