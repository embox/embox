/*
 * @file
 *
 * @date Sep 11, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>

const uint32_t __attribute__ ((section(".eh_frame_end"))) eh_frame_end = 0;

__attribute__((weak)) void __register_frame(void *begin) {
}

void register_eh_frame(void) {
	extern const char _eh_frame_begin;
	__register_frame((void *)&_eh_frame_begin);
}

void cxx_invoke_constructors(void) {
	extern const char _ctors_start, _ctors_end;
	typedef void (*ctor_func_t)(void);
	ctor_func_t *func = (ctor_func_t *) &_ctors_start;

	register_eh_frame();

	for ( ; func != (ctor_func_t *) &_ctors_end; func++) {
		(*func)();
	}
}
