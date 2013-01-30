/*
 * cxx_destruction_static.c
 *
 *  Created on: 30 janv. 2013
 *      Author: fsulima
 */

#include <embox/unit.h>
#include <stdio.h>

#include "cxx_destruction_signatures.h"

#define TABLE_SIZE OPTION_MODULE_GET(embox__lib__cxx__DestructionStatic,NUMBER,table_size)

struct atexit_func_entry_t
{
	void (*destructor_func)(void *);
	void *obj_ptr;
	void *dso_handle;
};

static struct atexit_func_entry_t __atexit_funcs[TABLE_SIZE];
static int __atexit_func_count = 0;

void *__dso_handle = 0;

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso)
{
	if (__atexit_func_count >= TABLE_SIZE) {
		printf("__cxa_atexit: static destruction table overflow.\n");
		// FIXME: assert(false);
		return -1;
	};
	__atexit_funcs[__atexit_func_count].destructor_func = f;
	__atexit_funcs[__atexit_func_count].obj_ptr = objptr;
	__atexit_funcs[__atexit_func_count].dso_handle = dso;
	__atexit_func_count++;
	return 0;
};

void __cxa_finalize(void *f) {
	int i = __atexit_func_count;
	if (!f)
	{
		while (i--)
		{
			if (__atexit_funcs[i].destructor_func)
			{
				(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
				__atexit_funcs[i].destructor_func = 0;
			};
		};
		__atexit_func_count = 0;
		return;
	};

	for ( ; i >= 0; --i)
	{
		if (__atexit_funcs[i].destructor_func == f)
		{
			(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			__atexit_funcs[i].destructor_func = 0;
		};
	};
}

void cxx_invoke_destructors(void) {
	__cxa_finalize(0);
}
