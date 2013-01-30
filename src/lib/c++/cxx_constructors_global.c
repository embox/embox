/*
 * cxx_constructors_global.c
 *
 *  Created on: 30 janv. 2013
 *      Author: fsulima
 */

#include <embox/unit.h>

EMBOX_UNIT_INIT(cxx_init);

EMBOX_UNIT_FINI(cxx_fini);

#include "cxx_invoke_constructors.h"
#include "cxx_invoke_destructors.h"
#include "cxx_app_startup_terminatioin.h"

static int cxx_init(void) {
	cxx_invoke_constructors();
	return 0;
}

static int cxx_fini(void) {
	cxx_invoke_destructors();
	return 0;
}

void cxx_app_startup(void) {
}

void cxx_app_termination(void) {
}
