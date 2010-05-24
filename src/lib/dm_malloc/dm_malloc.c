/**
 * @file
 *
 * @brief source file of dynamic memory allocator
 *
 * @date 02.05.2010
 *
 * @auther Michail Skorginskii
 *
 */

#include <lib/page_alloc.h>
#include <lib/multipage_alloc.h>
#include <lib/dm_malloc.h>
#include <lib/list.h>

/* adress of free memory block */
#define ADRESS(block) (block+sizeof(mem_block_t)+1)
/* the most big mem_block */
static size_t most_bigest_pa = 0;
/* memory list */
static LIST_HEAD(mem_list);

inline mem_block_t* allocate_mem_block(int pages)
{
	mem_block_t *tmp_alloc;
	tmp_alloc = (mem_block_t*) multipage_alloc(pages);
	if ( tmp_alloc == 0) {
		return 0;
	}
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

inline mem_block_t* eat_mem(size_t size, mem_block_t* ext) {
	if (ext->size == size) {
		return ext;
	}
	/* block from wich bit a mem */
	mem_block_t *tmp;

	/* memory reallocation */
	tmp =
		ext
		+ sizeof(mem_block_t)
		+ size;
	tmp->size =
		ext->size
		- size
		- 1;
	tmp->free = HOLE;
	/* reallocatation */
	ext->size = size;
	ext->free = PROC;
	/* add tail */
	list_add_tail(
		(struct list_head*) tmp,
		(struct list_head*) ext
	);
	return ext;
}

void* dm_malloc(size_t size) {
	/* we have a boats */
	if (most_bigest_pa >= size) {
		struct list_head *tmp;
		mem_block_t *tmp_mem;
		/* logic */
		list_for_each(tmp, &mem_list) {
			tmp_mem = (mem_block_t*) tmp;
			if (tmp_mem->free == HOLE && tmp_mem->size >= size) {
				tmp_mem = eat_mem(size, tmp_mem);
				return ADRESS(tmp_mem);
			}
		}
	}
	/* we hav't anought memory */
	else {
		mem_block_t *tmp;
		int pot = size / CONFIG_PAGE_SIZE;
		/* FIXME we must allocate by power of the 2 */
		/* logic */
		tmp = allocate_mem_block(pot);
		if (tmp == 0) {
			return 0;
		}
		list_add((struct list_head*)tmp, &mem_list);
		tmp = eat_mem(size, tmp);
		return ADRESS(tmp);
	}
	/* fuck, there are somthing wrong */
	return 0;
}

/*
 * free memory at ptr
 */
void dm_free(void *ptr) {
	mem_block_t *iterator;
	struct list_head *tmp;
	struct list_head *p;
	/* logic */
	list_for_each(p, &mem_list) {
		iterator = (mem_block_t *)p;
		if ( ADRESS(iterator) == ptr ) {

			iterator->free = HOLE;

			// one direction
			tmp = (struct list_head *) iterator;
			while ( ((mem_block_t *)tmp->next)->free != PROC) {
				tmp = tmp->next;
				iterator->size = iterator->size + ((mem_block_t *)tmp)->size;
				list_del(tmp);
			}

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
