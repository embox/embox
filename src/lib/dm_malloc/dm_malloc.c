/**
 * @file
 *
 * @brief source file of dynamic memory allocator
 *
 * @date 02.05.2010
 *
 * @auther Michail Skorginskii
 */

#include <lib/page_alloc.h>
#include <lib/multipage_alloc.h>
#include <lib/dm_malloc.h>
#include <lib/list.h>

/* some auxilraty stuff */
#define MALLOC_SIZE 8
#define PAGE_SIZE 0x1000
/* some stuff for easey programming */
inline static void allocate_mem_block(int pages);
inline static void eat_mem(size_t size, void *ext);
/* memory list */
static LIST_HEAD(mem_pool);
/* inited? */
bool inited=false;

void* dm_malloc(size_t size) {
	/* declarations */
	void *tmp;

	if (!inited) {
		allocate_mem_block(MALLOC_SIZE);
		!inited;
	}
	/* find block */
	list_for_each( ((struct list_head *) tmp), &mem_pool) {
		if ( ((tag_t *) (tmp - sizeof(tag_t)))->size >= size ) {
			eat_mem(size, (tmp - sizeof(tag_t)));
			/* and return */
			return tmp;
		}
	}
	/* fuck, there are somthing wrong */
	return 0;
}

void dm_free(void *ptr) {
	/* declarations */
	void * tmp;
	ptr = ptr - sizeof(tag_t);
	/* forward direction */
	tmp = ((tag_t*) ptr)->size + ptr;
	if ( ((tag_t*) tmp)->free == HOLE) {
		((tag_t *) ptr)->size += ((tag_t *) tmp)->size;
		list_del(((struct list_head *) tmp+sizeof(tag_t)));
	}
	/* backward dircetion */
	tmp = ptr - sizeof(tag_t);
	if ( ((tag_t*) tmp)->free == HOLE) {
		tmp = tmp - ((tag_t *)tmp)->size + sizeof(tag_t);

		((tag_t *) tmp)->size += ((tag_t *) ptr)->size;
		list_del(((struct list_head *) ptr+sizeof(tag_t)));
		ptr = tmp;
	}
	/* write tag's */
	((tag_t *)ptr)->free = HOLE;
	((tag_t *) (((tag_t *)ptr)->size + ptr))->free = HOLE;
	((tag_t *) (((tag_t *)ptr)->size + ptr))->size = ((tag_t *) ptr)->size;
	/* somthing wrong */
}

/* auxiliry function. allocate block of memory TODO add the nding memory work*/
inline static void allocate_mem_block(int pages) {
	/* declarations */
	void *tmp;

	tmp = multipage_alloc(pages);
	if ( tmp == 0) {
		return 0;
	}
	TRACE("PONG!");
	/* calculate size etc. */
	((tag_t*)tmp)->size =
		PAGE_SIZE
		* pages
		- sizeof(tag_t) * 2
		- sizeof(struct list_head);
	((tag_t*)tmp)->free = HOLE;

	/* add to list */
	list_add( ((struct list_head*) (tmp + sizeof(tag_t))) , &mem_pool);

	/* write structure at the end */
	/* while we do not need it i was not write it*/
}
/* auxiliry function. Eat mem. TODO add the ending memory work */
inline static void eat_mem(size_t size, void *ext) {
	/* decalations */
	void *tmp, *tmp_end;

	if ( ((tag_t*) ext)->size == size) {
		return ext;
	}

	/* memory reallocation */
	tmp =
		ext
		+ sizeof(tag_t)
		+ sizeof(struct list_head*)
		+ size;
	((tag_t *) tmp)->size =
		((tag_t *) ext)->size
		- size;
	((tag_t *) tmp)->free = HOLE;

	/* end mem */
	tmp_end = tmp
		/*+ sizeof(tag_t); you know why i do this?*/
		+ sizeof(struct list_head);
		/*- sizeof(tag_t); if not - see it*/
	((tag_t *) tmp_end)->size = ((tag_t *) tmp)->size;
	((tag_t *) tmp_end)->free = ((tag_t *) tmp)->free;

	/* reallocatation */
	((tag_t*)ext)->size = size;
	((tag_t*)ext)->free = PROC;

	/* add tail */
	list_add((struct list_head*) tmp, &mem_pool);
	list_del(ext);
	TRACE("PING!");
}

#undef PROC
#undef HOLE
#undef REPEAT
#undef ADRESS

