/**
 * @file
 *
 * @date 30 Jan 2013
 * @author: Felix Sulima
 */

#include <embox/unit.h>

extern void cxx_invoke_constructors(void);

EMBOX_UNIT_INIT(cxx_ctor_global_init);

static int cxx_ctor_global_init(void) {
	cxx_invoke_constructors();

	return 0;
}

void cxx_app_startup(void) {
}

void cxx_app_termination(void) {
}
