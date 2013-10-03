/*
 * @file
 *
 * @date Sep 11, 2012
 * @author: Anton Bondarev
 */


typedef void (*ctor_func_t)(void);

void cxx_invoke_constructors(void) {
	extern const char _ctors_start, _ctors_end;
	ctor_func_t *func;
	int n_entries;

	for (func = (ctor_func_t *) &_ctors_start, n_entries = 0;
			*func && (func != (ctor_func_t *) &_ctors_end);
			func++, n_entries++) {
				(*func)();
	}
}
