/**
 * @file
 *
 * @brief dunamic memory allocator
 *
 * @details TODO
 *
 * @date May 02, 2010
 *
 * @author Michail Skorginskii
 *
 * TODO
 * 1) make CONFIG macros
 * 2) details
 * 3) google Algorithms
 * 4) calloc && realloc
 * 5) make optimistation of dm_alloc()
 *
 * TODO fixed
 * 1)  write test
 * 2)  comments
 * 3)  typedef - wtf
 * 4)  think about exceptional situation
 * 5)  and write code
 * 6)  write .h
 * 7)  write Makefile: we don't need a make file
 * 8)  refactor init function
 * 9)  allocate wn_page's
 * 10) write doxygen comments
 * 11) think what alg is more good
 * 12) free
 */

#ifndef __DM_MALLOC_H
#define __DM_MALLOC_H

#define REPEAT(times) \
	for (int INC_REP=0; INC_REP < times; INC_REP++)
/**
 * Constant show that block is busy
 * @note 0 means that mem_block is busy
 */
#define PROC 0
/**
 * Constant show that block us free
 * @note 1 means that mem_block is free
 */
#define HOLE 1

/**
 * structure that describes
 * the memory block
 */
typedef struct mem_block {
	struct list_head *next, *prev;
	size_t size;
	bool free;
} mem_block_t;

/**
 * dynamic allocator
 * return pointer to mem vector
 * or return 0 if there are no memory
 */
void* dm_malloc(size_t size);

/**
 * free memory at *ptr
 */
void dm_free(void *ptr);

#endif /* __DM_MALLOC_H_ */


