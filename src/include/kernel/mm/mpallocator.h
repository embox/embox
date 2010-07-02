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

void*	mpalloc( size_t page_quantity );
void	mpfree( void* pointer );

#endif /* __MULTIPAGE_ALLOC_H_ */

