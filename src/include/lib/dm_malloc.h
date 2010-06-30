/**
 * @file
 *
 * @brief Dynamic memory allocator arbitrary size. Terehov style >_>
 *
 * @details TODO
 *
 * @date May 02, 2010
 *
 * @author Michail Skorginskii
 *
 * TODO
 * 1) Replace malloc module.
 * 2) Calloc && realloc.
 * 3) Uniformity
 */

#ifndef __DM_MALLOC_H
#define __DM_MALLOC_H

/**
 * auxiliry
 */
#define PAGE_SIZE 0x100

/**
 * auxiliry
 */
#define MALLOC_SIZE 0x2

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
 * to be writen
 */
typedef struct tag {
	size_t size;
	bool free;
} tag_t;

typedef struct tag_free {
	struct list_head *next, *prev;
	tag_t tag;
} tag_free_t;

/**
 * TODO
 * return adress
 * mem = begin of hole block
 */
#define ADRESS(begin) (void *) (begin + sizeof(tag_free_t))
/**
 *
 *
 */
#define END_TAG(begin) (tag_t*) (begin + sizeof(tag_free_t) + begin->tag.size - sizeof(tag_t))
/**
 *
 *
 */
#define BEGIN_TAG(end) (tag_free_t*) (end - end->size - sizeof(tag_free_t))

/**
 * Dynamic memory allocator arbitrary size.
 *
 * @param size is requested memory size.
 *
 * @return pointer to the memory of the requested size.
 * @retval 0 if there are no memory
 */
void* dm_malloc(size_t size);

/**
 * Free memory function.
 *
 * @param pointer at the memory, that must be free
 */
void dm_free(void *ptr);

/**
 * module info
 *
 * output information about all memory blocks
 * @note that it s auxiliary function
 */
void dm_info(void);

#endif /* __DM_MALLOC_H_ */

