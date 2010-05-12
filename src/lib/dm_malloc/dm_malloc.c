/**
 * @file
 *
 * @date 02.05.2010
 *
 * @auther Michail Skorginskii
 *
 */

/*TODO: write doxygen comments*/
/*TODO: read and rewrtie sources with code-style*/

/*TODO(fixed) write .h*/
/*TODO(fixed) write Makefile: we don't need a make file*/

#include <lib/page_alloc.h>
#include <lib/dm_malloc.h>

#define REPEAT(times) \
	for(int rep = 0; rep < times; rep++)
  /*page size is CONFIG_PAGE_SIZE*/

bool dm_inited = false;

/* allocated by page alloc memory size */
size_t alloc_mem_size;
/*
 * TODO: refactor init function
 * TODO: think about exceptional situation
 * 	 and write code
 * TODO: write some beatifull stuff
 */
void dm_malloc_init(void)
{
        alloc_mem_size = 0;

	// set cycled list
	mem = page_alloc();
	mem->adr  = mem + sizeof(mem_block_t) + 1;
	mem->size = CONFIG_PAGE_SIZE - sizeof(mem_block_t) - 1;

	mem->free = 'H';
	mem->prev = mem;
	mem_h = mem;
	REPEAT(7)
	{
		// allocate memory here!
		mem->next = page_alloc();
		mem->next->adr  = mem->next + sizeof(mem_block_t) + 1;
		mem->next->size = CONFIG_PAGE_SIZE - sizeof(mem_block_t) - 1;
		mem = mem->next;
		mem->next = mem_h;
	}
	dm_inited = true;
}

void* dm_malloc(size_t size)
{
	/* temporary act */
	if ( !dm_inited )
	{
		dm_malloc_init();
	}
	/* first feat */
	mem =  mem_h;
	while ( size >= mem->size )
	{
		mem = mem->next;
	}
	if ( size  >= mem->size )
	{
		TRACE("HELLO ! ^)");
		// allocate some new pages
		// or if size > CONFIG_PAGE_SIZE
		// merge two mems while size of mem <= mem
	}
	else
	{
                /*
		 *TRACE("\n mem adr = ");
		 *printf("%d", mem->adr);
		 *TRACE("\n mem pos = ");
		 *printf("%d", mem);
                 */
		mem->free = 'P';
		return mem->adr;
	}
}
void dm_free(void *ptr)
{
	/*TODO: google some good algorithms*/
	mem = mem_h;
	while ( mem->adr != ptr)
	{
		mem = mem->next;
	}
}
#undef REPEAT

/*TODO: write dynammic memory allocaton :) */


