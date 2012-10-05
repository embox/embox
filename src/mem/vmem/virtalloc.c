/**
 * @file
 * @brief
 *
 * @date 24.09.2012
 * @author Anton Bulychev
 */

#include <mem/vmem/virtalloc.h>
#include <mem/page.h>
#include <string.h>

// TODO: All this stuff

void *virt_alloc_table() {
	void *addr = page_alloc(1);
	memset(addr, 0 , PAGE_SIZE());
	return addr;
}

