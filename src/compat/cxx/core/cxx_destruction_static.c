/**
 * @file
 *
 * @date 30 Jan 2013
 * @author: Felix Sulima
 */

#include <embox/unit.h>
#include <stdio.h>

#define TABLE_SIZE OPTION_GET(NUMBER,table_size)

typedef void (*dtor_func_t)(void);

struct atexit_func_entry {
	void (*destructor_func)(void *);
	void *obj_ptr;
	void *dso_handle;
};

static struct atexit_func_entry atexit_funcs[TABLE_SIZE];
static int atexit_func_count = 0;

/* must be available for C++ ABI */
void *__dso_handle = 0;

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso) {
	if (atexit_func_count >= TABLE_SIZE) {
		printf("__cxa_atexit: static destruction table overflow.\n");
		return -1;
	}

	atexit_funcs[atexit_func_count].destructor_func = f;
	atexit_funcs[atexit_func_count].obj_ptr = objptr;
	atexit_funcs[atexit_func_count].dso_handle = dso;
	atexit_func_count++;

	return 0;
};

void __cxa_finalize(void *f) {
	int i = atexit_func_count;

	if (!f) {
		while (i--) {
			if (atexit_funcs[i].destructor_func) {
				(*atexit_funcs[i].destructor_func)(atexit_funcs[i].obj_ptr);
				atexit_funcs[i].destructor_func = 0;
			}
		}
		atexit_func_count = 0;
	} else {
		for ( ; i >= 0; --i) {
			if (atexit_funcs[i].destructor_func == f) {
				(*atexit_funcs[i].destructor_func)(atexit_funcs[i].obj_ptr);
				atexit_funcs[i].destructor_func = 0;
			}
		}
	}
}

void cxx_invoke_destructors(void) {
	extern const char _dtors_start, _dtors_end;
	typedef void (*dtor_func_t)(void);
	dtor_func_t *func = ((dtor_func_t *) &_dtors_end) - 1;

	/* There are two possible ways for destructors to be calls:
	 * 1. Through callbacks registered with __cxa_atexit.
	 * 2. From .fini_array section.  */

	/* Handle callbacks registered with __cxa_atexit first, if any.*/
	__cxa_finalize(0);

	/* Handle .fini_array, if any. Functions are executed in teh reverse order. */
	for ( ; func >= (dtor_func_t *) &_dtors_start; func--) {
		(*func)();
	}
}
