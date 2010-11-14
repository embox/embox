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

typedef struct block_info {
	struct list_head *next, *prev;
	size_t size;
	bool free;
}block_info_t;

void*	mpalloc( size_t page_quantity );
void	mpfree( void* pointer );

void    mpget_blocks_info(struct list_head* list);

#endif /* __MPALLOCATOR_H_ */

