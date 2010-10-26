/**
 * @file
 * @brief multipage allocator header
 * @details
 *
 * @date 28.04.10
 * @author Fedor Burdun
 */

#ifndef MPALLOCATOR_H_
#define MPALLOCATOR_H_

#include <types.h>

extern void*	mpalloc( size_t page_quantity );
extern void	mpfree( void* pointer );

#endif /* MPALLOCATOR_H_ */

