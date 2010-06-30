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

/* some stuff for easey programming */
inline static int  allocate_mem_block(int pages);
inline static void eat_mem(size_t size, tag_free_t *ext);
/* memory list */
static LIST_HEAD(mem_pool);
/* inited? */
bool inited=false;

void* dm_malloc(size_t size) {
	/* declarations */
	tag_free_t *tmp_begin;
	struct list_head *tmp_loop;
	/* we inited */
	if (!inited) {
		int expr = allocate_mem_block(MALLOC_SIZE);
		if ( expr  == 0 ) {
			return 0;
		}
		inited = !inited;
	}
	/* find block */
	list_for_each(tmp_loop, &mem_pool) {
		tmp_begin = (tag_free_t*) tmp_loop;
		TRACE("PING! ");
		if (tmp_begin->tag.size >= size) {
			eat_mem(size, tmp_begin );
			/* and return */
			return ADRESS(tmp_begin);
		}
	}
	TRACE(" \n === INFERNAL FUCK! === \n ");
	/* fuck, there are something wrong */
	return 0;
}

void dm_free(void *ptr) {
	/* declarations */
	tag_free_t *tmp_begin, *ptr_begin;
	tag_t      *tmp_end;
	ptr_begin = (tag_free_t *) ptr;
	/* forward direction */
	tmp_begin = (tag_free_t*) (END_TAG(ptr_begin) + sizeof(tag_t));
	if (tmp_begin->tag.free == HOLE) {
		/* del tag & moving adresses*/
		list_del((struct list_head *) tmp_begin);
		tmp_end = END_TAG(tmp_begin);
		/* rewrite tags */
		tmp_end->size = tmp_end->size + ptr_begin->tag.size;
		ptr_begin->tag.size = tmp_end->size;
	}
	/* backward dircetion */
	tmp_end = (tag_t*) (ptr_begin - sizeof(tag_t));
	if (tmp_end->free == HOLE) {
		/* del tag & moving adreses */
		tmp_begin = BEGIN_TAG(tmp_end);
		list_del((struct list_head*) ptr_begin);
		/* rewrite tag's */
		tmp_begin->tag.size = ptr_begin->tag.size + tmp_begin->tag.size;
		tmp_end = END_TAG(ptr_begin);
		tmp_end->size = tmp_begin->tag.size;
		/* move ptr */
		ptr = (void*) tmp_begin;
	}
	/* write tag's */
	tmp_begin = ptr;
	tmp_begin->tag.free = HOLE;
	tmp_end =  END_TAG(tmp_begin);
	tmp_end->free = HOLE;
}

/* auxiliry function. allocate block of memory TODO add the ending memory work */
inline static int allocate_mem_block(int pages) {
	/* declarations */
	tag_free_t *tmp_begin;
	tag_t* tmp_end;

	tmp_begin = (tag_free_t *) multipage_alloc(pages);
	if ( tmp_begin == 0) {
		return 0;
	}
	/* calculate size etc. */
	tmp_begin->tag.size =
		PAGE_SIZE * pages
		- sizeof(tag_free_t)
		- sizeof(tag_t);
	tmp_begin->tag.free = HOLE;
	/* write tag at the end */
	tmp_end = END_TAG(tmp_begin);
	tmp_end->size = tmp_begin->tag.size;
	tmp_end->free = HOLE;

	/* add to list */
	list_add((struct list_head*) tmp_begin, &mem_pool);
}
/* auxiliry function. Eat mem. add the ending memory work */
inline static void eat_mem(size_t size, tag_free_t* ext) {
	/* decalations */
	tag_free_t *tmp_begin;
	tag_t *tmp_end;
	size_t size_tmp;

	if ( ext->tag.size == size) {
		list_del((struct list_head*) ext);
		ext->tag.free = PROC;
	}

	/* delete ext */
	list_del((struct list_head*) ext);
	/* begin tag write */
	size_tmp = ext->tag.size;
	ext->tag.size = ext->tag.size - size;
	ext->tag.free = PROC;
	/* end tag write */
	tmp_end = END_TAG(ext);
	tmp_end->size = ext->tag.size;
	tmp_end->free = PROC;
	/* write new block  adresses */
	tmp_begin = (tag_free_t*) (tmp_end + sizeof(tag_t));
	tmp_end   = END_TAG(tmp_begin);
	/* write begin tag */
	/* think some more about adreses TODO */
	tmp_begin->tag.size = size_tmp - ext->tag.size;
	tmp_begin->tag.free = HOLE;
	/* write end tag */
	tmp_end->size = tmp_begin->tag.size;
	tmp_end->free = HOLE;
	/* add to list */
	list_add( (struct list_head*) tmp_begin, &mem_pool);
	printf("PONG!\n");
}

#undef PROC
#undef HOLE
#undef REPEAT
#undef ADRESS
