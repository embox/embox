/**
 * @file
 *
 * @date Sep 11, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>

typedef void (*ctor_func_t)(void);
typedef void (*dtor_func_t)(void);

extern void __cxa_finalize(void *f);

const uint32_t __attribute__((section(".eh_frame_end"))) eh_frame_end = 0;

__attribute__((weak)) void __register_frame(void *begin) {
}

void cxx_invoke_constructors(void) {
	extern const char _ctors_start, _ctors_end;
	extern const char _eh_frame_begin;

	ctor_func_t *func;

	__register_frame((void *)&_eh_frame_begin);

	for (func = (ctor_func_t *)&_ctors_start;
	     func != (ctor_func_t *)&_ctors_end; func++) {
		(*func)();
	}
}

void cxx_invoke_destructors(void) {
	extern const char _dtors_start, _dtors_end;

	dtor_func_t *func;

	/* There are two possible ways for destructors to be calls:
	 * 1. Through callbacks registered with __cxa_atexit.
	 * 2. From .fini_array section.  */

	/* Handle callbacks registered with __cxa_atexit first, if any.*/
	__cxa_finalize(0);

	/* Handle .fini_array, if any. Functions are executed in teh reverse order. */
	for (func = ((dtor_func_t *)&_dtors_end) - 1;
	     func >= (dtor_func_t *)&_dtors_start; func--) {
		(*func)();
	}
}
