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

#include <lib/page_alloc.h>
#include <lib/multipage_alloc.h>
#include <lib/dm_malloc.h>

/* has memory allocation has inited? */
bool dm_inited = false;
/* the most big mem_block */
static size_t most_bigest_pa;
/* memory list */
static LIST_HEAD(mem_list);
/**
 * return a pointer to allocated mem_block
 * pages
 */
inline mem_block_t* allocate_mem_block(int pages)
{
	mem_block_t *tmp_alloc;
	tmp_alloc = (mem_block_t*) multipage_alloc(pages);
	/*tmp_alloc = (mem_block_t*) page_alloc();*/
	if ( tmp_alloc == 0) {
		return 0;
	}
	tmp_alloc->adr  =
		tmp_alloc
		+ sizeof(mem_block_t)
		+ 1;
	tmp_alloc->size =
		CONFIG_PAGE_SIZE
		* pages
		- sizeof(mem_block_t)
		- 1;
	tmp_alloc->free = HOLE;
	if (most_bigest_pa < tmp_alloc->size) {
		most_bigest_pa = tmp_alloc->size;
	}
	return tmp_alloc;
}
/* forward under the tanks! */
int dm_malloc_init(void) {
	mem_block_t *tmp;
	REPEAT(4){
		tmp = allocate_mem_block(2);
		list_add((struct list_head*)tmp, &mem_list);
	}
	most_bigest_pa = CONFIG_PAGE_SIZE*8;
	dm_inited = true;
	return 0;
}



void* dm_malloc(size_t size) {
	/* we inited? */
	if ( !dm_inited ) {
		if (dm_malloc_init()) {
			return 0;
		}
	}
	/* we have a boats */
	if (most_bigest_pa >= size) {
		struct list_head *tmp;

		list_for_each(tmp, &mem_list) {
		if ( ((mem_block_t*)tmp)->free == HOLE
				&& ((mem_block_t*)tmp)->size >= size ) {
				// TODO eat so mem so we need
				((mem_block_t*)tmp)->free = PROC;
				return ((mem_block_t*)tmp)->adr;
			}
		}
	}
	/* we hav't anought memory */
	else {
		mem_block_t *tmp;

		tmp = allocate_mem_block(size / CONFIG_PAGE_SIZE);
		list_add((struct list_head*)tmp, &mem_list);
		// TODO return adress
	}
	/* fuck, there are somthing wrong */
	return 0;
}

/*
 * free memory at ptr
 */
void dm_free(void *ptr) {
	/*TODO: think what alg is more good*/
	/*TODO if there are neiboors, we must merge them */
	struct list_head *iterator;
	mem_block_t *block;

	list_for_each(iterator, &mem_list) {
		if ( ((mem_block_t*)iterator)->adr == ptr ) {
			block = (mem_block_t*)iterator;
			break;
		}
		block->free = HOLE;
	}
}

/*TODO: write dynammic memory allocaton :) */
#undef PROC
#undef HOLE
#undef REPEAT
