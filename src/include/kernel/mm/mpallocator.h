/**
 * @file
 * @brief multipage allocator header
 * @details
 *
 * @date 28.04.10
 * @author Fedor Burdun
 */

#ifndef __MPALLOCATOR_H_
#define __MPALLOCATOR_H_

#include <types.h>

void*	mpalloc( size_t page_quantity );
void	mpfree( void* pointer );

#endif /* __MPALLOCATOR_H_ */

