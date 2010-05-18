/**
 * @file
 *
 * @date 02.05.2010
 * @author Michail Skorginskii
 */
#ifndef __DM_MALLOC_H
#define __DM_MALLOC_H

/*TODO: write doxygen comments*/
/*TODO: make CONFIG macros*/

#include <lib/list.h>

/* PROC means that block is used by some process
 * HOLE means that block is free
 */
#define REPEAT(times) \
	for (int INC_REP=0; INC_REP < times; INC_REP++)
#define PROC 0
#define HOLE 1
typedef struct mem_block {
	struct list_head *next, *prev;
	void   *adr;
	size_t size;
	bool free;
} mem_block_t;

/**
 * allocation init
 * allocates 8 pages to make initial list
 */
int dm_malloc_init(void);

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

/* TODO: write calloc and realloc functions
 * TODO: google what is calloc and realloc
 */

#endif /* __DM_MALLOC_H_ */

/* TODOfixed: write test
 * TODOnotneed: comments
 * TODOfixed: typedef - wtf
 * TODOfixed: think about exceptional situation
 * 		and write code
 * TODOfixed write .h
 * TODOfixed write Makefile: we don't need a make file
 * TODOfixed refactor init function
 * TODOfixed allocate wn_page's
 * TODOfixed think what alg is more good
 */
