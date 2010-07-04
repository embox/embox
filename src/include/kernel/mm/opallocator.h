/**
 * @file
 * @brief page allocator header
 * @details
 *
 * @date 04.04.10
 * @author Fedor Burdun
 */

#ifndef __OPALLOCATOR_H_
#define __OPALLOCATOR_H_

#include <types.h>

/**
 * Allocates one physical memory page from the pool. We return pointer to page
 * marker it is hold at the begin of each block therefore we have pointer to
 * physical page which we returned.
 *
 * @return Address of allocated page
 * @retval Address of allocated page on success
 * @retval NULL on fail
 */
void *opalloc( void );

/**
 * Frees one page from the pool. This page must be allocated with
 * @link #page_alloc() @endlink function
 *
 * @param paddr pointer to physical page
 * @return Function doesn't return value
 *
 */
void opfree( void* paddr );

#endif /* __OPALLOCATOR_H_ */

