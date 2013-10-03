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

/* #if __EXCEPTIONS==1 */
/* 	//extern const char _eh_frame_start; */

 	extern void __do_global_ctors (void);
/* 	extern void *__deregister_frame_info (const void *); */
 	//__do_global_ctors();
/* #endif */

	for (func = (ctor_func_t *) &_ctors_start, n_entries = 0;
			*func && (func != (ctor_func_t *) &_ctors_end);
			func++, n_entries++) {
				(*func)();
	}

/* #if __EXCEPTIONS==1 */
/* 	//__deregister_frame_info (__EH_FRAME_BEGIN__); */
/* #endif */

}
