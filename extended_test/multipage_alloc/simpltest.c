/**
 * @file
 * @brief test page allocator
 * @details Some random or interactive tests for page allocator
 *
 * @date 09.05.10
 * @author Fedor Burdun
 */

#include <malloc.h>
#include <stdio.h>
#include <lib/multipage_alloc.h>

#include "config.h"

/**
 * pool
 */
char pool[PAGE_QUANTITY*CONFIG_PAGE_SIZE];
char *_heap_start = pool;
char *_heap_end = pool + PAGE_QUANTITY*CONFIG_PAGE_SIZE;

/**
 * print some information
 */
extern void multipage_info();


int main() {
	multipage_info();

	void * var = multipage_alloc(20);
	multipage_free(var);
}

