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
/*TODO: make optimisation of first fir*/

#include <lib/page_alloc.h>
#include <lib/multipage_alloc.h>
#include <lib/dm_malloc.h>

/* TODO: think about page free
 * There shoud be limit of memory
 * We shoud know what block is the pages
 */

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
/* may be we don't need it? */
int dm_malloc_init(void) {
	mem_block_t *tmp;
	REPEAT(4){
		tmp = allocate_mem_block(2);
		list_add((struct list_head*)tmp, &mem_list);
	}
	most_bigest_pa = CONFIG_PAGE_SIZE*2;
	dm_inited = true;
	return 0;
}

inline mem_block_t* eat_mem(size_t size, mem_block_t* ext) {
	if (ext->size == size) {
		return ext;
	}
	/* block from wich bit a mem */
	mem_block_t *big_tmp;

	/* memory reallocation */
	big_tmp =
		ext
		+ sizeof(mem_block_t)
		+ size;
	big_tmp->adr =
		big_tmp
		+ sizeof(mem_block_t)
		+ 1;
	big_tmp->size =
		ext->size
		- size
		- 1;
	big_tmp->free = HOLE;
	/* reallocatation */
	ext->adr =
		ext
		+ sizeof(mem_block_t)
		+ 1;
	ext->size = size;
	ext->free = PROC;
	/* add tail */
	list_add_tail(
		(struct list_head*) big_tmp,
		(struct list_head*) ext
	);
	return ext;
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
			if (
			   ((mem_block_t*)tmp) ->free    == HOLE
			&& ((mem_block_t*)tmp) ->size >= size ) {
				return eat_mem(size, ((mem_block_t*)tmp))->adr;
			}
		}
	}
	/* we hav't anought memory */
	else {
		mem_block_t *tmp;
		/* FIXME we must allocate by power of the 2 */
		tmp = allocate_mem_block(size / CONFIG_PAGE_SIZE);
		list_add((struct list_head*)tmp, &mem_list);
		return eat_mem(size, tmp)->adr;
	}
	/* fuck, there are somthing wrong */
	return 0;
}

/*
 * free memory at ptr
 */
void dm_free(void *ptr) {
	/*TODO if there are neiboors, we must merge them */
	mem_block_t *iterator;
	struct list_head *tmp;

	list_for_each((struct list_head*)iterator, &mem_list) {
		if ( iterator->adr == ptr ) {

			// one direction
			tmp = (struct list_head *) iterator;
			while ( ((mem_block_t *)tmp->next)->free != PROC) {
				tmp = tmp->next;
				iterator->size = iterator->size + ((mem_block_t *)tmp)->size;
				list_del(tmp);
			}
			iterator->free = HOLE;
			// second direction
			tmp = (struct list_head*) iterator;
			while ( ((mem_block_t *)tmp->prev)->free != PROC) {
				tmp = tmp->prev;
				((mem_block_t *)tmp)->size += iterator->size;
				iterator = tmp;
				list_del(tmp->next);
			}
			break;
		}
	}
}

/*TODO: write dynammic memory allocaton :) */
#undef PROC
#undef HOLE
#undef REPEAT
