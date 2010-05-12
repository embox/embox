/**
 * @file
 *
 * @date 02.05.2010
 * @author Michail Skorginskii
 */
#ifndef __DYNALLOC_H
#define __DYNALLOC_H

/*TODO: write doxygen comments*/
/*TODO: make CONFIG macros*/

/*TODO: write test*/

/*
 * mem block struct
 * TODO: comments
 * TODO: typedef - wtf
 */
#include <lib/list.h>
/*
 * TODO: free to bool. bool is too big for this thing
 */
typedef struct mem_block {
	char 	free;
	void    *adr;
	size_t  size;
	struct mem_block *next;
	struct mem_block *prev;
} mem_block_t;
/*  */
/**
 * all memory
 */
static mem_block_t *mem;
static mem_block_t *mem_h;

/**
 * allocation init
 * nothing returns, just initiliaze
 */
void dm_malloc_init(void);

/**
 * dynamic allocator
 * return pointer to mem vector
 */
void* dm_alloc(size_t size);

/**
 * TODO: write comments
 *
 */
void dm_free(void *ptr);

/* TODO: write calloc and realloc functions
 * TODO: google what is calloc and realloc
 */
#endif /* __DYNALLOC_H_ */

