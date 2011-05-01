/**
 * @file
 * @brief Dynamic memory allocator
 *
 * @date 02.05.2010
 * @author Michail Skorginskii
 */

#include <kernel/mm/mpallocator.h>
#include <kernel/mm/kmalloc.h>
#include <lib/list.h>
#include <stdlib.h>

/**
 * to be writen
 */
typedef struct tag {
	size_t size;
	bool   free;
} tag_t;

typedef struct tag_free {
	struct list_head *next, *prev;
	tag_t tag;
} tag_free_t;

/**
 * Show that block is busy.
 *
 * @note 0 means that mem_block is busy.
 * @note this macross is undefined in the end of this module.
 */
#define PROC true

/**
 * Show that block is free.
 *
 * @note 1 means that mem_block is free.
 * @note this macross is undefined in the end of this module.
 */
#define HOLE false

/**
 * return address
 */
#define ADDRESS(begin) (void*) (begin + sizeof(tag_free_t))

#define END_TAG(begin) (tag_t*) (begin + begin->tag.size + sizeof(tag_free_t))

#define BEGIN_TAG(end) (tag_free_t*) (end - end->size - sizeof(tag_free_t))

/* some stuff for easey programming */
inline static int allocate_mem_block(int pages);
inline static void eat_mem(size_t size, tag_free_t *ext);
/* memory list */
static LIST_HEAD(mem_pool);
/* inited? */
bool inited = false;

tag_free_t* begin_pool_ptr;

void* kmalloc(size_t size) {
	tag_free_t *tmp_begin;
	struct list_head *tmp_loop;

	if (size == 0)
		return NULL;
	/* some securuty actions */
	size = (size + sizeof(tag_t) - 1) & -(size_t) sizeof(tag_t);

	/* we inited */
	if (!inited) {
		int expr = allocate_mem_block(CONFIG_MALLOC_SIZE);
		if (expr == 0) {
			return 0;
		}
		inited = !inited;
	}

	/* find block */
	list_for_each(tmp_loop, &mem_pool) {
		tmp_begin = (tag_free_t*) tmp_loop;
		if (tmp_begin->tag.size >= size) {
			eat_mem(size, tmp_begin);
			/* and return */
			return ADDRESS(tmp_begin);
		}
	}
	TRACE("\n === INFERNAL FUCK! === \n");
	/* fuck, there are something wrong */
	return 0;
}

void kfree(void *ptr) {
	tag_free_t *tmp_begin, *ptr_begin;
	tag_t *tmp_end;

	if (ptr == NULL)
		return;

	ptr_begin = (tag_free_t *) (ptr - sizeof(tag_free_t));

	if (ptr_begin->tag.free == HOLE || ptr_begin->tag.size == 0)
		return;
	/* forward direction */
	tmp_begin = (tag_free_t*) ((void*) END_TAG(ptr_begin) + sizeof(tag_t));
	if ((void*) tmp_begin - (void*) begin_pool_ptr < CONFIG_PAGE_SIZE
			* CONFIG_MALLOC_SIZE && tmp_begin->tag.free == HOLE) {
		/* del tag & moving adresses*/
		list_del((struct list_head *) tmp_begin);
		tmp_end = END_TAG(tmp_begin);
		/* rewrite tags */
		tmp_end->size = tmp_end->size + ptr_begin->tag.size
				+ sizeof(tag_free_t) + sizeof(tag_t);
		ptr_begin->tag.size = tmp_end->size;
	}
	/* backward direction */
	tmp_end = (tag_t*) ((void*) ptr_begin - sizeof(tag_t));
	if ((void*) tmp_end - (void*) begin_pool_ptr > 0 && tmp_end->free == HOLE) {
		/* del tag & moving adreses */
		tmp_begin = BEGIN_TAG(tmp_end);
		list_del((struct list_head*) tmp_begin);
		/* rewrite tag's */
		tmp_begin->tag.size = ptr_begin->tag.size + tmp_begin->tag.size
				+ sizeof(tag_free_t) + sizeof(tag_t);
		tmp_end = END_TAG(tmp_begin);
		tmp_end->size = tmp_begin->tag.size;
		/* move ptr */
		ptr_begin = tmp_begin;
	}
	/* write tag's */

	ptr_begin->tag.free = HOLE;
	tmp_end = END_TAG(ptr_begin);
	tmp_end->free = HOLE;
	list_add((struct list_head*) ptr_begin, &mem_pool);
}

/* auxiliary function. allocate block of memory TODO add the ending memory work */
inline static int allocate_mem_block(int pages) {
	tag_free_t *tmp_begin;
	tag_t* tmp_end;

	tmp_begin = (tag_free_t *) mpalloc(pages);
	begin_pool_ptr = tmp_begin;
	if (tmp_begin == 0) {
		return 0;
	}
	/* calculate size etc. */
	tmp_begin->tag.size = CONFIG_PAGE_SIZE * pages - sizeof(tag_free_t)
			- sizeof(tag_t);
	tmp_begin->tag.free = HOLE;
	/* write tag at the end */
	tmp_end = END_TAG(tmp_begin);
	tmp_end->size = tmp_begin->tag.size;
	tmp_end->free = HOLE;

	/* add to list */
	list_add((struct list_head*) tmp_begin, &mem_pool);
	return 1;
}
/* auxiliary function. Eat mem. add the ending memory work */
inline static void eat_mem(size_t size, tag_free_t* ext) {
	tag_free_t *tmp_begin;
	tag_t *tmp_end;
	size_t size_tmp;

	if (ext->tag.size - size <= sizeof(tag_free_t) + sizeof(tag_t)) {
		list_del((struct list_head*) ext);
		ext->tag.free = PROC;
		tmp_end = END_TAG(ext);
		tmp_end->free = PROC;
		return;
	}
	/* delete ext */
	list_del((struct list_head*) ext);
	/* begin tag write */
	size_tmp = ext->tag.size;
	ext->tag.size = size;
	ext->tag.free = PROC;
	/* end tag write */
	tmp_end = END_TAG(ext);

	tmp_end->size = ext->tag.size;
	tmp_end->free = PROC;
	/* write new block  adresses */
	tmp_begin = (tag_free_t*) ((void*) tmp_end + sizeof(tag_t));

	/* write begin tag */
	tmp_begin->tag.size = size_tmp - ext->tag.size - sizeof(tag_free_t)
			- sizeof(tag_t);

	tmp_end = END_TAG(tmp_begin);
	tmp_begin->tag.free = HOLE;
	/* write end tag */
	tmp_end->size = tmp_begin->tag.size;
	tmp_end->free = HOLE;
	/* add to list */
	list_add((struct list_head*) tmp_begin, &mem_pool);
}

/**
 * return list of free and busy blocks in heap
 */
void kmget_blocks_info(struct list_head* list) {
	block_info_t* tmp_info;
	tag_free_t* tmp_ptr;
	int pool_size = CONFIG_MALLOC_SIZE * CONFIG_PAGE_SIZE;

	if (!inited) {
		int expr = allocate_mem_block(CONFIG_MALLOC_SIZE);
		if (expr == 0) {
			return;
		}
		inited = !inited;
	}

	tmp_ptr = begin_pool_ptr;

	do {
		tmp_info = (block_info_t*) malloc(sizeof(block_info_t));
		tmp_info->size = tmp_ptr->tag.size;
		tmp_info->free = !(tmp_ptr->tag.free);
		list_add((struct list_head*) tmp_info, list);
		tmp_ptr = (tag_free_t*) ((void*) END_TAG(tmp_ptr) + sizeof(tag_t));
	} while ((void*) tmp_ptr - (void*) begin_pool_ptr < pool_size);
}

#undef PROC
#undef HOLE
#undef REPEAT
#undef ADDRESS
