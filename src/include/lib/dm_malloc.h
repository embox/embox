/**
 * @file
 *
 * @brief Dynamic memory allocator arbitrary size.
 *
 * @details TODO
 *
 * @date May 02, 2010
 *
 * @author Michail Skorginskii
 *
 * TODO
 * 1) Replace malloc module.
 * 2) Retvals with names.
 * 3) Calloc && realloc.
 * 4) Page free problem.
 * 5) Memory adress problem.
 * 6) Uniformity  JUST FUKING TODO IT!
 */

#ifndef __DM_MALLOC_H
#define __DM_MALLOC_H

/**
 * Repeat code under brackets { } N times.
 *
 * @param how much time macros repeat code.
 *
 * @note this macros is undefined in the end of this module.
 */
#define REPEAT(times) \
	for (int INC_REP=0; INC_REP < times; INC_REP++)

/**
 * Show that block is busy.
 *
 * @note 0 means that mem_block is busy.
 * @note this macross is undefined in the end of this module.
 */
#define PROC 0

/**
 * Show that block is free.
 *
 * @note 1 means that mem_block is free.
 * @note this macross is undefined in the end of this module.
 */
#define HOLE 1

/**
 * Calculate begining of the memory block.
 *
 * @param memory block. type is 'mem_block_t'
 *
 * @return begin of the memory block
 *
 * @note it is not a function ;)
 */
#define ADRESS(block) (block+sizeof(mem_block_t)+1)

/**
 * Memory block structure.
 *
 * @note struct list_head *next, *prev must be always at the top in the same order.
 */
typedef struct mem_block {
	struct list_head *next, *prev;
	size_t size;
	bool free;
} mem_block_t;

/**
 * Internal memory representation.
 */
//static LIST_HEAD(mem_list);

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
 *
 * @retval 0 if everething is OK
 * @retval -1 if there are no memory block with such adress
 */
int dm_free(void *ptr);

#endif /* __DM_MALLOC_H_ */


