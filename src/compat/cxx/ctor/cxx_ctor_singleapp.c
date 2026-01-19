/**
 * @file
 *
 * @date 30 Jan 2013
 * @author: Felix Sulima
 */

#include <embox/unit.h>

extern void cxx_invoke_constructors(void);
extern void cxx_invoke_destructors(void);
extern void __register_frame(void *begin); /* libgcc */
extern const char _eh_frame_begin;

EMBOX_UNIT_INIT(cxx_ctor_singleapp_init);

static int cxx_ctor_singleapp_init(void) {
	__register_frame((void *)&_eh_frame_begin);

	return 0;
}

void cxx_app_startup(void) {
	cxx_invoke_constructors();
}

void cxx_app_termination(void) {
	cxx_invoke_destructors();
}
