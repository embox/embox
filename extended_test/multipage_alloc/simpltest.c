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

/**
 */
extern void multipage_init();


int main() {
	multipage_init();
	multipage_info();

	printf("Try alloc 1 pages\n");
	void * var3 = multipage_alloc(1);
	printf("ALLOC: %08x\n",var3);
	multipage_info();
	printf("Try alloc 2 pages\n");
	void * var = multipage_alloc(2);
	printf("ALLOC: %08x\n",var);
	multipage_info();
	printf("Try alloc 1 pages\n");
	void * var2 = multipage_alloc(1);
	printf("ALLOC: %08x\n",var2);
	multipage_info();

	printf("Try free 2's\n");
	multipage_free(var);
	multipage_info();
	printf("Try free 3's\n");
	multipage_free(var2);
	multipage_info();
	printf("Try free 1's\n");
	multipage_free(var3);
	multipage_info();
}

