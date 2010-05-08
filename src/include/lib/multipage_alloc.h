/**
 * @file
 * @brief multipage allocator header
 * @details
 *
 * @date 28.04.10
 * @author Fedor Burdun
 */

#ifndef __MULTIPAGE_ALLOC_H_
#define __MULTIPAGE_ALLOC_H_

#include <types.h>

void* multipage_alloc(size_t page_quantity);
void  multipage_free (void * pointer);

#endif /* __MULTIPAGE_ALLOC_H_ */

